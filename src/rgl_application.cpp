#include <clocale>

#include <QTimer>
#include <QNetworkProxy>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QLoggingCategory>
#include <QDir>
#include <QStyle>
#include <QStyleHints>
#include <QTranslator>

#include <rbl_arguments_parser.h>
#include <rbl_job_manager.h>
#include <rbl_logger.h>
#include <rbl_utils.h>
#include <rbl_version.h>

#include "rgl_application.h"
#include "rgl_cloud_file_manager.h"
#include "rgl_crash_report_dialog.h"
#include "rgl_file_updater.h"
#include "rgl_locker_handler.h"
#include "rgl_logger_handler.h"
#include "rgl_message_box.h"
#include "rgl_open_ssl_csr_task.h"
#include "rgl_progress_handler.h"
#include "rgl_software_manager_dialog.h"
#include "rgl_software_update_checker.h"
#include "rgl_style.h"

RApplication::RApplication(int &argc, char **argv, bool initializeSettings)
    : QApplication{argc,argv}
    , applicationSettings{nullptr}
    , actionList{nullptr}
    , mainWindow{nullptr}
{
    // Needed for printf functions family to work correctly.
    setlocale(LC_ALL,"C");

    QCoreApplication::setOrganizationName(RVendor::familyName());
    QCoreApplication::setOrganizationDomain(RVendor::wwwDomain());
    QCoreApplication::setApplicationName(RVendor::shortName());
    QGuiApplication::setApplicationDisplayName(RVendor::name());

    RLockerHandler::initialize();
    RLoggerHandler::initialize();
    RProgressHandler::initialize();

    if (initializeSettings)
    {
        this->applicationSettings = new RApplicationSettings(this);
        this->applicationSettings->setDirApplicationPath(this->applicationDirPath());
    }

    QObject::connect(this,&RApplication::started,this,&RApplication::onStarted);

    QTimer::singleShot(0, this, SIGNAL(started()));
}

RApplication *RApplication::instance() noexcept
{
    return qobject_cast<RApplication*>(QApplication::instance());
}

const RCloudSessionManager *RApplication::getCloudSessionManager() const
{
    return this->cloudSessionManager;
}

RCloudSessionManager *RApplication::getCloudSessionManager()
{
    return this->cloudSessionManager;
}

RCloudConnectionHandler *RApplication::getCloudConnectionHandler()
{
    return this->cloudConnectionHandler;
}

const RApplicationSettings *RApplication::getApplicationSettings() const
{
    return this->applicationSettings;
}

RApplicationSettings *RApplication::getApplicationSettings()
{
    return this->applicationSettings;
}

const RActionList *RApplication::getActionList() const
{
    return this->actionList;
}

RActionList *RApplication::getActionList()
{
    return this->actionList;
}

const QMainWindow *RApplication::getMainWindow() const
{
    return this->mainWindow;
}

QMainWindow *RApplication::getMainWindow()
{
    return this->mainWindow;
}

void RApplication::applyTranslation(const QString &languageCode)
{
    RLogger::info("Applying translation \'%s\'\n",languageCode.toUtf8().constData());

    this->applyFormat(this->applicationSettings->getFormat());

    for (const QString &translatorName : std::as_const(this->translatorNames))
    {
        QString translatorFile = ":/i18n/" + translatorName + "/" + languageCode + ".qm";

        RLogger::info("Loading translator file \'%s\'.\n",translatorFile.toUtf8().constData());
        QTranslator *translator = new QTranslator(this);
        if (!translator->load(translatorFile))
        {
            RLogger::warning("Failed to load translator file \'%s\'.\n",translatorFile.toUtf8().constData());
            delete translator;
        }
        else
        {
            this->installTranslator(translator);
        }
    }
}

void RApplication::applyFormat(RApplicationSettings::Format format)
{
    switch (format)
    {
        case RApplicationSettings::Format::System:
        {
            QLocale::setDefault(QLocale::system());
            break;
        }
        case RApplicationSettings::Format::Language:
        {
            QLocale::setDefault(RApplicationSettings::languageCodeToLanguage(this->applicationSettings->getLanguageCode()));
            break;
        }
        case RApplicationSettings::Format::C:
        {
            QLocale::setDefault(QLocale::c());
            break;
        }
        default:
        {
            RLogger::warning("Unknown format \'%d\'\n", format);
            break;
        }
    }
}

void RApplication::updateFiles()
{
    RLogger::info("Preparing files update.\n");
    RLogger::indent();

    RFileUpdater *pFileUpdater = new RFileUpdater;

    for (const QString &sourceName : std::as_const(this->updaterSourceDirs))
    {
#ifdef Q_OS_DARWIN
        QDir dataSrcDir(QDir::cleanPath(QDir(this->applicationDirPath()).filePath(QString("..") + QDir::separator() + "Resources" + QDir::separator() + sourceName)));
#else
        QDir dataSrcDir(QDir::cleanPath(QDir(this->applicationDirPath()).filePath(QString("..") + QDir::separator() + sourceName)));
#endif
        QDir dataDstDir(RApplicationSettings::getAppDataDir(sourceName));

        RLogger::info("Source directory: \'%s\'\n",dataSrcDir.absolutePath().toUtf8().constData());
        RLogger::info("Destination directory: \'%s\'\n",dataDstDir.absolutePath().toUtf8().constData());

        if (dataSrcDir.exists())
        {
            QStringList files = dataSrcDir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDir::Name);
            for (const QString &file : std::as_const(files))
            {
                RLogger::info("Adding file: \'%s\' -> \'%s\'\n",dataSrcDir.filePath(file).toUtf8().constData(),dataDstDir.filePath(file).toUtf8().constData());
                pFileUpdater->addFile(dataSrcDir.filePath(file),dataDstDir.filePath(file));
            }
        }
    }
    RLogger::unindent();
    RJobManager::getInstance().submit(pFileUpdater);
}

void RApplication::validateCloudClientCertificate()
{
    for (const QString &cloudSessionName : this->cloudSessionManager->getSessionNames())
    {
        RCloudSessionInfo cloudSessionInfo = this->cloudSessionManager->findSession(cloudSessionName);
        QString clientCertificateFile = cloudSessionInfo.getClientKeyStore().getCertificateFile();
        if (!QFileInfo::exists(clientCertificateFile))
        {
            continue;
        }

        QPair<QDateTime,QDateTime> clientCertificateValidity = RTlsTrustStore::findValidity(clientCertificateFile);
        if (clientCertificateValidity.second.isNull())
        {
            continue;
        }

        QString clientCertificateCommonName = RTlsTrustStore::findCN(clientCertificateFile);
        QDateTime now = QDateTime::currentDateTime();
        QDateTime nowPlusExpiry = now.addDays(this->applicationSettings->getCloudClientCertificateExpiryDays());
        if (clientCertificateValidity.second < now)
        {
            QString certificateWarningMessage = tr("Cloud client certificate has expired.")
            + "<ul>"
                + "<li><strong>" + tr("Session name") + ":</strong> " + cloudSessionName + "</li>"
                + "<li><strong>" + tr("Common name") + ":</strong> " + clientCertificateCommonName + "</li>"
                + "<li><strong>" + tr("Expiration date") + ":</strong> " + clientCertificateValidity.second.toString() + "</li>"
                + "</ul>";
            RMessageBox::warning(this->mainWindow,tr("Certificate has expired"), certificateWarningMessage);
        }
        else if (clientCertificateValidity.second < nowPlusExpiry)
        {
            if (this->applicationSettings->getCloudClientCertificateRenew())
            {
                RLogger::info("Attempting to renew Cloud client certificate.\n");
                RCloudSessionInfo cloudSessionInfo = this->cloudSessionManager->findSession(cloudSessionName);
                if (cloudSessionInfo.isValid())
                {
                    ROpenSslCsrTask *pOpenSslCsrTask = new ROpenSslCsrTask(this->applicationSettings,
                                                                           this->cloudConnectionHandler,
                                                                           cloudSessionInfo,
                                                                           this->mainWindow);
                    QObject::connect(pOpenSslCsrTask,&ROpenSslCsrTask::finished,this,&RApplication::onOpenSslCsrTaskFinished);
                    QObject::connect(pOpenSslCsrTask,&ROpenSslCsrTask::finished,pOpenSslCsrTask,&ROpenSslCsrTask::deleteLater);
                    QObject::connect(pOpenSslCsrTask,&ROpenSslCsrTask::failed,this,&RApplication::onOpenSslCsrTaskFailed);
                    QObject::connect(pOpenSslCsrTask,&ROpenSslCsrTask::failed,pOpenSslCsrTask,&ROpenSslCsrTask::deleteLater);
                }
            }
            else
            {
                QString certificateWarningMessage = tr("Cloud client certificate will soon expire.")
                + "<ul>"
                    + "<li><strong>" + tr("Session name") + ":</strong> " + cloudSessionName + "</li>"
                    + "<li><strong>" + tr("Common name") + ":</strong> " + clientCertificateCommonName + "</li>"
                    + "<li><strong>" + tr("Expiration date") + ":</strong> " + clientCertificateValidity.second.toString() + "</li>"
                    + "</ul>";
                RMessageBox::warning(this->mainWindow,tr("Certificate will expire"), certificateWarningMessage);
            }
        }
    }
}

void RApplication::sendUsageReport(const QString &logFile) const
{
    if (!QFile::exists(logFile))
    {
        RLogger::warning("Cannot prepare a report because log file \'%s\' does not exist.\n",logFile.toUtf8().constData());
        return;
    }

    RLogger::info("Preparing a report from the log file \'%s\'.\n",logFile.toUtf8().constData());

    QFile file(logFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        RLogger::error("Failed to opent the log file \'%s\'.\n",logFile.toUtf8().constData());
        return;
    }

    QString fileContent;
    try
    {
        QTextStream fileStream(&file);
        while (!fileStream.atEnd())
        {
            if (!fileContent.isEmpty())
            {
                fileContent.append('\n');
            }
            fileContent.append(fileStream.readLine());
        }

        RReportRecord reportRecord;
        reportRecord.setReport(fileContent);
        reportRecord.setComment("Report from last run");
        RCloudReportSender::sendReport(this->applicationSettings,reportRecord);
    }
    catch (...)
    {
        RLogger::error("Unknown error while reading the log file.\n");
    }
    file.close();
}

void RApplication::onStarted()
{
    // Process command line arguments.
    try
    {
        QList<RArgumentOption> validOptions;
        validOptions.append(RArgumentOption("log-qt",RArgumentOption::Switch,QVariant(),"Switch on qt debug log level",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-ssl",RArgumentOption::Switch,QVariant(),"Switch on ssl debug log level",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-debug",RArgumentOption::Switch,QVariant(),"Switch on debug log level",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-trace",RArgumentOption::Switch,QVariant(),"Switch on trace log level",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("log-threads",RArgumentOption::Switch,QVariant(),"Enable printing thread IDs",RArgumentOption::Logger,false));
        validOptions.append(RArgumentOption("reset-defaults",RArgumentOption::Switch,QVariant(),"Reset all settings to defaults",RArgumentOption::Optional,false));

        RArgumentsParser argumentsParser(RApplication::arguments(),validOptions,true);

        if (argumentsParser.isSet("help"))
        {
            argumentsParser.printHelp();
            this->exit(0);
            return;
        }

        if (argumentsParser.isSet("version"))
        {
            argumentsParser.printVersion();
            this->exit(0);
            return;
        }

        if (argumentsParser.isSet("log-debug"))
        {
            RLogger::getInstance().setLevel(R_LOG_LEVEL_DEBUG);
        }
        if (argumentsParser.isSet("log-trace"))
        {
            RLogger::getInstance().setLevel(R_LOG_LEVEL_TRACE);
        }
        if (argumentsParser.isSet("log-threads"))
        {
            RLogger::getInstance().setPrintThreadIdEnabled(true);
        }
        if (argumentsParser.isSet("log-qt") ||
            argumentsParser.isSet("log-ssl"))
        {
            RLogger::installQtMessageHandler();
            RLogger::debug("Qt message handler installed\n");

            if (argumentsParser.isSet("log-qt"))
            {
                QLoggingCategory::setFilterRules("qt.*=true");
                RLogger::debug("All Qt logging enabled\n");
            }
            else if (argumentsParser.isSet("log-ssl"))
            {
                QLoggingCategory::setFilterRules(
                    "qt.network.ssl.debug=true\n"
                    "qt.network.ssl.info=true\n"
                    "qt.network.ssl.warning=true\n"
                    "qt.tlsbackend.ossl.debug=true\n"
                    "qt.tlsbackend.ossl.info=true\n"
                    "qt.tlsbackend.ossl.warning=true"
                    );
                RLogger::debug("SSL logging enabled\n");
            }
        }
        if (argumentsParser.isSet("reset-defaults"))
        {
            this->applicationSettings->clear();
        }

        this->filesToLoad = argumentsParser.getFiles();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to parse command line options. %s\n",rError.getMessage().toUtf8().constData());
        this->exit(1);
        return;
    }

    RStyle::applyStyle(this->applicationSettings->getStyle(),this->applicationSettings->getColorScheme());
    this->applyTranslation(this->applicationSettings->getLanguageCode());

    // Connect to aboutToQuit signal
    QObject::connect(this,&RApplication::aboutToQuit,this,&RApplication::onAboutToQuit);
    // Connect to style changed signal
    QObject::connect(this->applicationSettings,&RApplicationSettings::styleChanged,this,&RApplication::onStyleChanged);
    // Connect to color scheme changed signal.
    QObject::connect(this->applicationSettings,&RApplicationSettings::colorSchemeChanged,this,&RApplication::onColorSchemeChanged);
    // Connect to color scheme changed signal.
    QObject::connect(QGuiApplication::styleHints(),&QStyleHints::colorSchemeChanged,this,&RApplication::onColorSchemeChanged);
    // Connect to language changed signal
    QObject::connect(this->applicationSettings,&RApplicationSettings::languageChanged,this,&RApplication::onLanguageChanged);
    // Connect to format changed signal
    QObject::connect(this->applicationSettings,&RApplicationSettings::formatChanged,this,&RApplication::onFormatChanged);

    // Rotate log file.
    QString rotatedLogFile = this->applicationSettings->getLogFileName() + "-" + QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
    if (QFile::exists(this->applicationSettings->getLogFileName()))
    {
        if (!QFile::rename(this->applicationSettings->getLogFileName(),rotatedLogFile))
        {
            RLogger::error("Failed to rename the log file \'%s\' to \'%s\'\n",
                           this->applicationSettings->getLogFileName().toUtf8().constData(),
                           rotatedLogFile.toUtf8().constData());
        }
    }

    // Initialize lock file.
    qint64 lockPid;
    QString lockHostname;
    QString lockAppname;

    this->pLockFile = new QLockFile(this->applicationSettings->getTmpDir() + QDir::separator() + RVendor::packageName() + ".lck");

    bool previousLockStillValid = this->pLockFile->getLockInfo(&lockPid,&lockHostname,&lockAppname);

    this->pLockFile->setStaleLockTime(0);
    if (!this->pLockFile->tryLock(0))
    {
        QString lockMessage = "Failed to start.\nApplication " + lockAppname + " @ " + lockHostname + " (PID = " + QString::number(lockPid) + ") seems to be already running.\n";
        RMessageBox::warning(nullptr,tr("Application is already running"), lockMessage);

        this->exit(1);
        return;
    }

    // Initialize session
    this->cloudSessionManager = new RCloudSessionManager(this);

    // Initialize connection handler.
    this->cloudConnectionHandler = new RCloudConnectionHandler(this->cloudSessionManager,this->applicationSettings,this);

    // Initialize cloud session
    QString cloudSessionFileName = this->applicationSettings->getCloudSessionFileName();
    if (!cloudSessionFileName.isEmpty() && QFile::exists(cloudSessionFileName))
    {
        try
        {
            this->cloudSessionManager->read(cloudSessionFileName);
        }
        catch (const RError &rError)
        {
            RLogger::warning("Failed to read the cloud session file \'%s\'. ERROR: %s\n",cloudSessionFileName.toUtf8().constData(),rError.getMessage().toUtf8().constData());
        }
    }

    // Register data directory to be updated in case of new software version
    this->updaterSourceDirs.append("data");

    // Initialize user objects
    this->initialize();

    // Set log file
    RLogger::getInstance().setFile(this->applicationSettings->getLogFileName());
    // Unhalt logger
    RLoggerHandler::unhalt();

    // Set use system proxy settings
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QStringList sysInfo;
    sysInfo.append(QString("Build ABI: %1").arg(QSysInfo::buildAbi()));
    sysInfo.append(QString("Build Cpu Architecture: %1").arg(QSysInfo::buildCpuArchitecture()));
    sysInfo.append(QString("Current Cpu Architecture: %1").arg(QSysInfo::currentCpuArchitecture()));
    sysInfo.append(QString("Kernel Type: %1").arg(QSysInfo::kernelType()));
    sysInfo.append(QString("Kernel Version: %1").arg(QSysInfo::kernelVersion()));
    sysInfo.append(QString("Machine HostName: %1").arg(QSysInfo::machineHostName()));
    sysInfo.append(QString("Product Name: %1").arg(QSysInfo::prettyProductName()));
    sysInfo.append(QString("Product Type: %1").arg(QSysInfo::productType()));
    sysInfo.append(QString("Product Version: %1").arg(QSysInfo::productVersion()));
    sysInfo.append(QString("Platform: %1").arg(QGuiApplication::platformName()));
    sysInfo.append(QString("Machine precision (float):  %14g").arg(double(RConstants::findMachineFloatEpsilon())));
    sysInfo.append(QString("Machine precision (double): %14g").arg(RConstants::findMachineDoubleEpsilon()));

    QStringList qtInfo;
    qtInfo.append(QString("Version: %1").arg(QLibraryInfo::version().toString()));
    qtInfo.append(QString("Plugins paths: %1").arg(QLibraryInfo::paths(QLibraryInfo::PluginsPath).join(",")));
    qtInfo.append(QString("Libraries paths: %1").arg(QLibraryInfo::paths(QLibraryInfo::LibrariesPath).join(",")));
    qtInfo.append(QString("Library executables paths: %1").arg(QLibraryInfo::paths(QLibraryInfo::LibraryExecutablesPath).join(",")));
    qtInfo.append(QString("Application library paths: %1").arg(QCoreApplication::libraryPaths().join(",")));
    qtInfo.append(QString("SSL support: %1").arg(QSslSocket::supportsSsl()?"true":"false"));
    qtInfo.append(QString("SSL available backends: %1").arg(QSslSocket::availableBackends().join(",")));
    qtInfo.append(QString("SSL active backend: %1").arg(QSslSocket::activeBackend()));

    QStringList appInfo;
    appInfo.append(QString("Style: %1").arg(QApplication::style()->objectName()));
    appInfo.append(QString("Color scheme: %1").arg(RStyle::colorSchemeToText(QGuiApplication::styleHints()->colorScheme())));
    appInfo.append(QString("Application directory: \"%1\"").arg(this->applicationDirPath()));
    appInfo.append(QString("Data directory: \"%1\"").arg(this->applicationSettings->getDataDir()));
    appInfo.append(QString("Certificates directory: \"%1\"").arg(this->applicationSettings->getCertDir()));
    appInfo.append(QString("Document directory: \"%1\"").arg(this->applicationSettings->getDocDir()));
    appInfo.append(QString("Log directory: \"%1\"").arg(this->applicationSettings->getLogDir()));
    appInfo.append(QString("Temporary directory: \"%1\"").arg(this->applicationSettings->getTmpDir()));
    appInfo.append(QString("Session directory: \"%1\"").arg(this->applicationSettings->getSessionDir()));
    appInfo.append(QString("Help doc directory: \"%1\"").arg(this->applicationSettings->getHelpDir()));
    appInfo << this->userAppInfo;

    // Print basic information
    RLogger::info("System information\n");
    RLogger::indent();
    for (const QString &infoText : sysInfo)
    {
        RLogger::info("%s\n",infoText.toUtf8().constData());
    }
    RLogger::unindent(false);
    RLogger::info("Qt library information\n");
    RLogger::indent();
    for (const QString &infoText : std::as_const(qtInfo))
    {
        RLogger::info("%s\n",infoText.toUtf8().constData());
    }
    RLogger::unindent(false);
    for (const QString &infoText : std::as_const(appInfo))
    {
        RLogger::info("%s\n",infoText.toUtf8().constData());
    }
    // Check current version.
    RLogger::info("Checking current against last used software version.\n");
    RLogger::indent();
    RLogger::info("Last used version: %s\n",this->applicationSettings->getStoredVersion().toString().toUtf8().constData());
    RLogger::info("Currently used version: %s\n",RVendor::version().toString().toUtf8().constData());
    RLogger::unindent(false);

    if (RVendor::version() > this->applicationSettings->getStoredVersion())
    {
        // Newer version is being executed.

        // Perform files update.
        this->updateFiles();
    }

    // Check client certificate validity
    this->validateCloudClientCertificate();

    // Software update checker
    RSoftwareUpdateChecker *softwareUpdateChecker = new RSoftwareUpdateChecker(this->applicationSettings,this);
    QObject::connect(softwareUpdateChecker,&RSoftwareUpdateChecker::softwareAvailable,this,&RApplication::onSoftwareAvailable);

    // Cloud file manager
    new RCloudFileManager(this->cloudConnectionHandler,this->applicationSettings,this);

    if (previousLockStillValid)
    {
        RCrashReportDialog crashReportDialog(this->applicationSettings,rotatedLogFile,this->mainWindow);
        crashReportDialog.exec();
    }
    else
    {
        if (this->applicationSettings->getSoftwareSendUsageInfo())
        {
            // Send software usage report
            this->sendUsageReport(rotatedLogFile);
        }
    }
}

void RApplication::onAboutToQuit()
{
    // Store cloud session
    QString cloudSessionFileName = this->applicationSettings->getCloudSessionFileName();
    try
    {
        this->cloudSessionManager->write(cloudSessionFileName);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to write the cloud session file \'%s\'. ERROR: %s\n",cloudSessionFileName.toUtf8().constData(),error.getMessage().toUtf8().constData());
    }

    this->finalize();

    this->pLockFile->unlock();
    delete this->pLockFile;

    // Stop logger
    RLogger::info("Stoping logger\n");
    RLoggerHandler::halt();

    // Delete main window
    delete this->mainWindow;
}

void RApplication::onStyleChanged(const QString &styleName)
{
    RStyle::applyStyle(styleName,this->applicationSettings->getColorScheme());
}

void RApplication::onColorSchemeChanged(Qt::ColorScheme colorScheme)
{
    QString styleName(this->style()->name());
    if (styleName.isEmpty())
    {
        styleName = this->applicationSettings->getStyle();
    }
    RStyle::applyStyle(styleName,colorScheme);
}

void RApplication::onLanguageChanged(const QString &languageCode)
{
    this->applyTranslation(languageCode);
}

void RApplication::onFormatChanged(RApplicationSettings::Format format)
{
    this->applyFormat(format);
}

void RApplication::onSoftwareAvailable(QList<RFileInfo> fileInfoList)
{
    R_LOG_TRACE_IN;

    QString infoMessage = tr("Following software updates are available");

    infoMessage += "<ul>";
    for (const RFileInfo &fileInfo : fileInfoList)
    {
        infoMessage += "<li>";
        infoMessage += "<tt>" + fileInfo.getPath() + "</tt>";
        infoMessage += "</li>";
    }
    infoMessage += "</ul>";
    infoMessage += tr("Would you like to open the software manager?");

    if (RMessageBox::question(this->mainWindow,tr("Software updates available"),infoMessage) == RMessageBox::Yes)
    {
        RSoftwareManagerDialog softwareManagerDialog(this->applicationSettings,this->mainWindow);
        softwareManagerDialog.exec();
    }

    R_LOG_TRACE_OUT;
}

void RApplication::onOpenSslCsrTaskFinished(RCloudSessionInfo sessionInfo)
{
    this->cloudSessionManager->insertSession(sessionInfo);
}

void RApplication::onOpenSslCsrTaskFailed(QString message)
{
    RLogger::error("OpenSSL CSR task has failed. %s\n",message.toUtf8().constData());
}
