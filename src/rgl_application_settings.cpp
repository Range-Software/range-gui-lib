#include <QStyleFactory>
#include <QDir>
#include <QDesktopServices>
#include <QImageWriter>
#include <QStandardPaths>

#include <rbl_logger.h>
#include <rbl_utils.h>

#include "rgl_application_settings.h"

const QString RApplicationSettings::Style::WindowsDark = "Windows-Dark";
const QString RApplicationSettings::Style::FusionDark = "Fusion-Dark";
const QString RApplicationSettings::Style::FusionRange = "Fusion-Range";
const QString RApplicationSettings::Style::Fusion = "Fusion";

const QString RApplicationSettings::Language::En::Code = "en";
const QString RApplicationSettings::Language::En::Name = "English";
const QString RApplicationSettings::Language::Sk::Code = "sk";
const QString RApplicationSettings::Language::Sk::Name = "Slovensky";

const QString RApplicationSettings::cloudSessionFileName("cloudSession.json");
const QString RApplicationSettings::logFileName("application.log");
const QString RApplicationSettings::licenseFileName("LICENSE.txt");
const QString RApplicationSettings::releaseNotesFileName("RELEASE_NOTES.txt");
const QString RApplicationSettings::opensslCnfFileName("openssl.cnf");

const QString RApplicationSettings::helpDirKey = "application/helpDir";
const QString RApplicationSettings::styleKey = "application/style";
const QString RApplicationSettings::languageKey = "application/language";
const QString RApplicationSettings::formatKey = "application/format";
const QString RApplicationSettings::toolbarIconSizeKey = "application/toolbarIconSize";
const QString RApplicationSettings::cloudRefreshTimeoutKey = "cloud/refreshTimeout";
const QString RApplicationSettings::softwareSendUsageInfoKey = "cloud/softwareSendUsageInfo";
const QString RApplicationSettings::softwareCheckUpdatesKey = "cloud/softwareCheckUpdates";
const QString RApplicationSettings::proxyTypeKey = "proxy/proxyType";
const QString RApplicationSettings::proxyHostKey = "proxy/proxyHost";
const QString RApplicationSettings::proxyPortKey = "proxy/proxyPort";
const QString RApplicationSettings::proxyUserKey = "proxy/proxyUser";
const QString RApplicationSettings::proxyPasswordKey = "proxy/proxyPassword";
const QString RApplicationSettings::userFullNameKey = "user/fullName";
const QString RApplicationSettings::userEmailKey = "user/email";
const QString RApplicationSettings::userTerritoryKey = "user/territory";
const QString RApplicationSettings::opensslToolPathKey = "tools/opensslToolPath";
const QString RApplicationSettings::opensslConfPathKey = "tools/opensslConfPath";

RApplicationSettings::RApplicationSettings(QObject *parent)
    : QSettings(parent)
{
    this->sync();
    this->storedVersion = RVersion(this->value("version").toString());
    this->setValue("version",QVariant(RVendor::version().toString()));
}

void RApplicationSettings::setDirApplicationPath(const QString &applicationPath)
{
    this->applicationDirPath = applicationPath;
}

QString RApplicationSettings::getCloudSessionFileName() const
{
    return this->getSessionDir() + QDir::separator() + RApplicationSettings::cloudSessionFileName;
}

QString RApplicationSettings::getLogFileName() const
{
    return this->getLogDir() + QDir::separator() + RApplicationSettings::logFileName;
}

QString RApplicationSettings::findLicenseFileName() const
{
    QDir dir(this->applicationDirPath);
    dir.cdUp();
#ifdef Q_OS_DARWIN
    dir.cd("Resources");
#endif
    dir.cd("doc");
    return dir.filePath(RApplicationSettings::licenseFileName);
}

QString RApplicationSettings::findReleaseNotesFileName() const
{
    QDir dir(this->applicationDirPath);
    dir.cdUp();
#ifdef Q_OS_DARWIN
    dir.cd("Resources");
#endif
    dir.cd("doc");
    return dir.filePath(RApplicationSettings::releaseNotesFileName);
}

QString RApplicationSettings::findOpensslConfPath() const
{
    QDir dir(this->findEtcDir());
    return dir.filePath(RApplicationSettings::opensslCnfFileName);
}

QString RApplicationSettings::findEtcDir() const
{
    QDir dir(this->applicationDirPath);
    dir.cdUp();
#ifdef Q_OS_DARWIN
    dir.cd("Resources");
#endif
    return dir.filePath("etc");
}

QString RApplicationSettings::findHelpDir() const
{
    QDir dir(this->applicationDirPath);
    dir.cdUp();
#ifdef Q_OS_DARWIN
    dir.cd("Resources");
#endif
    return dir.filePath("help");
}

QString RApplicationSettings::findPixmapsDir() const
{
    QDir dir(this->applicationDirPath);
    dir.cdUp();
#ifdef Q_OS_DARWIN
    dir.cd("Resources");
#endif
    return dir.filePath("pixmaps");
}

const RVersion &RApplicationSettings::getStoredVersion() const
{
    return this->storedVersion;
}

QStringList RApplicationSettings::getSupportedImageFormats()
{
    QStringList formats;

    QList<QByteArray> supportedFormats = QImageWriter::supportedImageFormats();
    for (const QByteArray &format : std::as_const(supportedFormats))
    {
        formats.append(format.toLower());
    }

    return formats;
}

QStringList RApplicationSettings::getSupportedVideoFormats()
{
    QStringList formats;

    formats.append("3gp2");
    formats.append("3gpp");
    formats.append("avi");
    formats.append("m2ts");
    formats.append("m4v");
    formats.append("mkv");
    formats.append("mov");
    formats.append("mp4");
    formats.append("mpg");

    formats.sort();

    return formats;
}

QStringList RApplicationSettings::getSupportedDocumentFormats()
{
    QStringList formats;

    formats.append("txt");
    formats.append("log");
    formats.append("pdf");
    formats.append("odf");
    formats.append("md");
    formats.append("html");

    return formats;
}

QString RApplicationSettings::getDir(const QString &topDir, const QString &subDir)
{
    QDir dir(topDir);

    if (!dir.exists(subDir))
    {
        QString fullPath(dir.filePath(subDir));
        RLogger::info("Creating directory \'%s\'\n",fullPath.toUtf8().constData());
        if (!dir.mkpath(fullPath))
        {
            RLogger::error("Failed to create a directory \'%s\'\n",fullPath.toUtf8().constData());
        }
    }
    if (!dir.cd(subDir))
    {
        RLogger::error("Failed to change to a sub-directory \'%s\' in \'%s\'\n",
                  subDir.toUtf8().constData(),
                  dir.path().toUtf8().constData());
    }

    return dir.path();
}

QString RApplicationSettings::getHelpDir() const
{
    return this->value(RApplicationSettings::helpDirKey,this->findHelpDir()).toString();
}

void RApplicationSettings::setHelpDir(const QString &helpDir)
{
    this->setValue(RApplicationSettings::helpDirKey, helpDir);
}

QString RApplicationSettings::getStyle() const
{
    return this->value(RApplicationSettings::styleKey,RApplicationSettings::getDefaultStyle()).toString();
}

void RApplicationSettings::setStyle(const QString &style)
{
    QString newStyle = RApplicationSettings::getStyles().contains(style) ? style : RApplicationSettings::getDefaultStyle();
    this->setValue(RApplicationSettings::styleKey, newStyle);
    emit this->styleChanged(newStyle);
}

QString RApplicationSettings::getLanguageCode() const
{
    return this->value(RApplicationSettings::languageKey,RApplicationSettings::getDefaultLanguageCode()).toString();
}

void RApplicationSettings::setLanguageCode(const QString &languageCode)
{
    QString newLanguageCode = RApplicationSettings::getLanguageCodes().contains(languageCode) ? languageCode : RApplicationSettings::getDefaultLanguageCode();
    this->setValue(RApplicationSettings::languageKey, newLanguageCode);
    emit this->languageChanged(newLanguageCode);
}

RApplicationSettings::Format RApplicationSettings::getFormat() const
{
    QString defaultFormatCode = RApplicationSettings::formatToCode(RApplicationSettings::getDefaultFormat());
    return RApplicationSettings::codeToFormat(this->value(RApplicationSettings::formatKey,defaultFormatCode).toString());
}

void RApplicationSettings::setFormat(Format newFormat)
{
    this->setValue(RApplicationSettings::formatKey, RApplicationSettings::formatToCode(newFormat));
    emit this->formatChanged(newFormat);
}

int RApplicationSettings::getToolbarIconSize() const
{
    return this->value(RApplicationSettings::toolbarIconSizeKey,RApplicationSettings::getDefaultToolbarIconSize()).toInt();
}

void RApplicationSettings::setToolbarIconSize(int toolbarIconSize)
{
    this->setValue(RApplicationSettings::toolbarIconSizeKey, toolbarIconSize);
    emit this->toolbarIconSizeChanged(toolbarIconSize);
}

uint RApplicationSettings::getCloudRefreshTimeout() const
{
    return this->value(RApplicationSettings::cloudRefreshTimeoutKey,RApplicationSettings::getDefaultCloudRefreshTimeout()).toUInt();
}

void RApplicationSettings::setCloudRefreshTimeout(uint cloudRefreshTimeout)
{
    this->setValue(RApplicationSettings::cloudRefreshTimeoutKey, cloudRefreshTimeout);
    emit this->cloudRefreshTimeoutChanged(cloudRefreshTimeout);
}

uint RApplicationSettings::getSoftwareSendUsageInfo() const
{
    return this->value(RApplicationSettings::softwareSendUsageInfoKey,RApplicationSettings::getDefaultSoftwareSendUsageInfo()).toBool();
}

void RApplicationSettings::setSoftwareSendUsageInfo(bool cloudSendUsageInfo)
{
    this->setValue(RApplicationSettings::softwareSendUsageInfoKey, cloudSendUsageInfo);
    emit this->cloudSendUsageInfoChanged(cloudSendUsageInfo);
}

uint RApplicationSettings::getSoftwareCheckUpdates() const
{
    return this->value(RApplicationSettings::softwareCheckUpdatesKey,RApplicationSettings::getDefaultSoftwareCheckUpdates()).toBool();
}

void RApplicationSettings::setSoftwareCheckUpdates(bool checkSoftwareUpdates)
{
    this->setValue(RApplicationSettings::softwareCheckUpdatesKey, checkSoftwareUpdates);
    emit this->cloudSendUsageInfoChanged(checkSoftwareUpdates);
}

RHttpProxySettings RApplicationSettings::getProxySettings() const
{
    RHttpProxySettings proxySettings;

    proxySettings.setType(RHttpProxySettings::Type(this->value(RApplicationSettings::proxyTypeKey,RHttpProxySettings::SystemProxy).toInt()));
    proxySettings.setHost(this->value(RApplicationSettings::proxyHostKey,QString()).toString());
    proxySettings.setPort(this->value(RApplicationSettings::proxyPortKey,80).toUInt());
    proxySettings.setUser(this->value(RApplicationSettings::proxyUserKey,QString()).toString());
    proxySettings.setPassword(this->value(RApplicationSettings::proxyPasswordKey,QString()).toString());

    return proxySettings;
}

void RApplicationSettings::setProxySettings(const RHttpProxySettings &proxySettings)
{
    this->setValue(RApplicationSettings::proxyTypeKey, proxySettings.getType());
    this->setValue(RApplicationSettings::proxyHostKey, proxySettings.getHost());
    this->setValue(RApplicationSettings::proxyPortKey, proxySettings.getPort());
    this->setValue(RApplicationSettings::proxyUserKey, proxySettings.getUser());
    this->setValue(RApplicationSettings::proxyPasswordKey, proxySettings.getPassword());
    emit this->proxySettingsChanged(proxySettings);
}

QString RApplicationSettings::getUserFullName() const
{
    return this->value(RApplicationSettings::userFullNameKey,QString()).toString();
}

void RApplicationSettings::setUserFullName(const QString &userFullName)
{
    this->setValue(RApplicationSettings::userFullNameKey, userFullName);
}

QString RApplicationSettings::getUserEmail() const
{
    return this->value(RApplicationSettings::userEmailKey,QString()).toString();
}

void RApplicationSettings::setUserEmail(const QString &userEmail)
{
    this->setValue(RApplicationSettings::userEmailKey, userEmail);
}

QString RApplicationSettings::getUserTerritory() const
{
    return this->value(RApplicationSettings::userTerritoryKey,RApplicationSettings::getDefaultUserTerritory()).toString();
}

void RApplicationSettings::setUserTerritory(const QString &userTerritory)
{
    this->setValue(RApplicationSettings::userTerritoryKey, userTerritory);
}

QString RApplicationSettings::getOpensslToolPath() const
{
    return this->value(RApplicationSettings::opensslToolPathKey,RApplicationSettings::getDefaultOpensslToolPath()).toString();
}

void RApplicationSettings::setOpensslToolPath(const QString &opensslToolPath)
{
    this->setValue(RApplicationSettings::opensslToolPathKey, opensslToolPath);
}

QString RApplicationSettings::getOpensslConfPath() const
{
    return this->value(RApplicationSettings::opensslConfPathKey,RApplicationSettings::findOpensslConfPath()).toString();
}

void RApplicationSettings::setOpensslConfPath(const QString &opensslConfPath)
{
    this->setValue(RApplicationSettings::opensslConfPathKey, opensslConfPath);
}

QString RApplicationSettings::getHomeDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

QString RApplicationSettings::getAppHomeDir()
{
    return RApplicationSettings::getDir(RApplicationSettings::getDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),RVendor::familyName()),RVendor::shortName());
}

QString RApplicationSettings::getAppDataDir(const QString &subDir)
{
    return RApplicationSettings::getDir(RApplicationSettings::getAppHomeDir(),subDir);
}

QString RApplicationSettings::getDataDir()
{
    return RApplicationSettings::getAppDataDir("data");
}

QString RApplicationSettings::getCertDir()
{
    return RApplicationSettings::getAppDataDir("cert");
}

QString RApplicationSettings::getDocDir()
{
    return RApplicationSettings::getAppDataDir("doc");
}

QString RApplicationSettings::getEtcDir()
{
    return RApplicationSettings::getAppDataDir("etc");
}

QString RApplicationSettings::getLogDir()
{
    return RApplicationSettings::getAppDataDir("log");
}

QString RApplicationSettings::getTmpDir()
{
    return RApplicationSettings::getAppDataDir("tmp");
}

QString RApplicationSettings::getSessionDir()
{
    return RApplicationSettings::getAppDataDir("session");
}

QStringList RApplicationSettings::getStyles()
{
    QStringList styles = QStyleFactory::keys();
    styles.append(RApplicationSettings::Style::FusionDark);
    styles.append(RApplicationSettings::Style::FusionRange);
    styles.append(RApplicationSettings::Style::WindowsDark);
    styles.sort();
    return styles;
}

QStringList RApplicationSettings::getLanguageCodes()
{
    QStringList languageCodes;
    languageCodes.append(RApplicationSettings::Language::En::Code);
    languageCodes.append(RApplicationSettings::Language::Sk::Code);
    return languageCodes;
}

QStringList RApplicationSettings::getLanguageNames()
{
    QStringList languageNames;
    languageNames.append(RApplicationSettings::Language::En::Name);
    languageNames.append(RApplicationSettings::Language::Sk::Name);
    return languageNames;
}

QString RApplicationSettings::languageNameToCode(const QString &languageName)
{
    if (languageName == RApplicationSettings::Language::En::Name)
    {
        return RApplicationSettings::Language::En::Code;
    }
    else if (languageName == RApplicationSettings::Language::Sk::Name)
    {
        return RApplicationSettings::Language::Sk::Code;
    }
    return RApplicationSettings::getDefaultLanguageCode();
}

QLocale::Language RApplicationSettings::languageCodeToLanguage(const QString &languageCode)
{
    if (languageCode == RApplicationSettings::Language::En::Code)
    {
        return QLocale::C;
    }
    else if (languageCode == RApplicationSettings::Language::Sk::Code)
    {
        return QLocale::Slovak;
    }
    return QLocale::C;
}

RApplicationSettings::Format RApplicationSettings::codeToFormat(const QString &code)
{
    if (code == "language")
    {
        return Format::Language;
    }
    else if (code == "posix")
    {
        return Format::C;
    }
    else
    {
        return Format::System;
    }
}

QString RApplicationSettings::formatToCode(Format format)
{
    switch (format)
    {
        case Language: return "language";
        case C: return "posix";
        case System:
        default:  return "system";
    }
}

QString RApplicationSettings::formatToName(Format format)
{
    switch (format)
    {
        case Language: return tr("Language");
        case C: return tr("POSIX");
        case System:
        default:  return tr("System");
    }
}

QString RApplicationSettings::getDefaultStyle()
{
    return RApplicationSettings::getStyles().at(0);
    //    return MainSettings::SystemDefault;
}

QString RApplicationSettings::getDefaultLanguageCode()
{
    return RApplicationSettings::getLanguageCodes().at(0);
}

RApplicationSettings::Format RApplicationSettings::getDefaultFormat()
{
    return RApplicationSettings::Format::System;
}

int RApplicationSettings::getDefaultToolbarIconSize()
{
    return 32;
}

uint RApplicationSettings::getDefaultCloudRefreshTimeout()
{
    return 5000;
}

bool RApplicationSettings::getDefaultSoftwareSendUsageInfo()
{
    return true;
}

bool RApplicationSettings::getDefaultSoftwareCheckUpdates()
{
    return true;
}

const QString RApplicationSettings::getDefaultUserTerritory()
{
    return QLocale::territoryToCode(QLocale::system().territory()).toUpper();
}

const QString RApplicationSettings::getDefaultPrivateKeyPath()
{
    QDir certDir(RApplicationSettings::getCertDir());
    return certDir.absoluteFilePath("private-key.pem");
}

const QString RApplicationSettings::getDefaultCsrPath()
{
    QDir certDir(RApplicationSettings::getCertDir());
    return certDir.absoluteFilePath("csr.pem");
}

const QString RApplicationSettings::getDefaultCertificatePath()
{
    QDir certDir(RApplicationSettings::getCertDir());
    return certDir.absoluteFilePath("certificate.pem");
}

const QString RApplicationSettings::getDefaultCaCertificatePath()
{
    QDir certDir(RApplicationSettings::getCertDir());
    return certDir.absoluteFilePath("ca-certificate.pem");
}

const QString RApplicationSettings::getDefaultOpensslToolPath()
{
    QStringList appLocations = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    appLocations.append(QCoreApplication::applicationDirPath());
    QString executable = QStandardPaths::findExecutable("openssl",appLocations);
    if (executable.isEmpty())
    {
        executable = QStandardPaths::findExecutable("openssl");
    }
    return executable;
}

QColor RApplicationSettings::getDefaultBackgroundColor(const QString &style)
{
    if (style == RApplicationSettings::Style::Fusion)
    {
        return QColor(34,68,102);
    }
    else if (style == RApplicationSettings::Style::FusionRange)
    {
        return QColor(48,0,0);
    }
    else if (style == RApplicationSettings::Style::FusionDark)
    {
        return QColor(53,53,53);
    }
    else if (style == RApplicationSettings::Style::WindowsDark)
    {
        return QColor(53,53,53);
    }
    else
    {
        return QColor(34,68,102);
    }
}
