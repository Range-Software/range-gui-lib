#include <QDateTime>
#include <QDir>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSslCertificate>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QInputDialog>

#include <rbl_error.h>
#include <rbl_logger.h>
#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include <rcl_cloud_tool_action.h>
#include <rcl_file_tools.h>

#include "rgl_file_chooser_button.h"
#include "rgl_message_box.h"
#include "rgl_open_ssl_csr_dialog.h"
#include "rgl_open_ssl_tool.h"

ROpenSslCsrDialog::ROpenSslCsrDialog(RCloudConnectionHandler *connectionHandler,
                                     const QString &openSslToolPath,
                                     const QString &openSslCnfPath,
                                     const QString &newKeyPath,
                                     const QString &newKeyPassword,
                                     const QString &newCsrPath,
                                     const QString &newCertificatePath,
                                     const QMap<QString, QString> &subjectMap,
                                     QWidget *parent)
    : QDialog{parent}
    , openSslToolPath{openSslToolPath}
    , openSslCnfPath{openSslCnfPath}
    , newKeyPath{newKeyPath}
    , newKeyPassword{newKeyPassword}
    , newCsrPath{newCsrPath}
    , newCertificatePath{newCertificatePath}
{
    this->publicCloudClient = connectionHandler->createPublicClient(this);
    this->privateCloudClient = connectionHandler->createPrivateClient(this);

    QObject::connect(this->privateCloudClient,&RCloudClient::signedCertificateAvailable,this,&ROpenSslCsrDialog::onSignedCertificate);
    QObject::connect(this->publicCloudClient,&RCloudClient::signedCertificateAvailable,this,&ROpenSslCsrDialog::onSignedCertificate);
    QObject::connect(this->publicCloudClient,&RCloudClient::userRegistered,this,&ROpenSslCsrDialog::onUserRegistered);

    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");
    QIcon generateIcon(":/icons/file/pixmaps/range-new.svg");
    QIcon directRequestIcon(":/icons/action/pixmaps/range-add.svg");
    QIcon emailRequestIcon(":/icons/action/pixmaps/range-add.svg");
    QIcon registerUserIcon(":/icons/action/pixmaps/range-new.svg");


    this->setWindowTitle(tr("OpenSSL certificate signing request"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QGroupBox *newFilesGroupBox = new QGroupBox(tr("New identity files"));
    // newFilesGroupBox.set
    mainLayout->addWidget(newFilesGroupBox);

    QFormLayout *newFilesLayout = new QFormLayout;
    newFilesGroupBox->setLayout(newFilesLayout);

    RFileChooserButton *newPrivateKeyFileButton = new RFileChooserButton(QString(),RFileChooserButton::SaveFile);
    newPrivateKeyFileButton->setFileName(this->newKeyPath);
    newFilesLayout->addRow(tr("Private key file") + ":",newPrivateKeyFileButton);

    QLineEdit *newPrivateKeyPasswordEdit = new QLineEdit(this->newKeyPassword);
    newPrivateKeyPasswordEdit->setEchoMode(QLineEdit::Password);
    newFilesLayout->addRow(tr("Password") + ":",newPrivateKeyPasswordEdit);

    RFileChooserButton *newCsrFileButton = new RFileChooserButton(QString(),RFileChooserButton::SaveFile);
    newCsrFileButton->setFileName(this->newCsrPath);
    newFilesLayout->addRow(tr("CSR file") + ":",newCsrFileButton);

    RFileChooserButton *newCertificateFileButton = new RFileChooserButton(QString(),RFileChooserButton::SaveFile);
    newCertificateFileButton->setFileName(this->newCertificatePath);
    newFilesLayout->addRow(tr("Certificate file") + ":",newCertificateFileButton);

    QGroupBox *subjectGroupBox = new QGroupBox(tr("Certificate subject field"));
    mainLayout->addWidget(subjectGroupBox);

    QFormLayout *subjectLayout = new QFormLayout;
    subjectGroupBox->setLayout(subjectLayout);

    this->territoryCombo = new RTerritoryComboBox(subjectMap.value(ROpenSslTool::CertificateSubject::Country::key));
    subjectLayout->addRow("C:",this->territoryCombo);

    this->stateEdit = new QLineEdit(subjectMap.value(ROpenSslTool::CertificateSubject::State::key));
    this->stateEdit->setPlaceholderText(tr("State"));
    subjectLayout->addRow("ST:",stateEdit);

    this->locationEdit = new QLineEdit(subjectMap.value(ROpenSslTool::CertificateSubject::Location::key));
    this->locationEdit->setPlaceholderText(tr("Location"));
    subjectLayout->addRow("L:",this->locationEdit);

    this->organizationEdit = new QLineEdit(subjectMap.value(ROpenSslTool::CertificateSubject::Organization::key));
    this->organizationEdit->setPlaceholderText(tr("Organization"));
    subjectLayout->addRow("O:",this->organizationEdit);

    this->organizationUnitEdit = new QLineEdit(subjectMap.value(ROpenSslTool::CertificateSubject::OrganizationUnit::key));
    this->organizationUnitEdit->setPlaceholderText(tr("Organization unit"));
    subjectLayout->addRow("OU:",this->organizationUnitEdit);

    this->commonNameEdit = new QLineEdit(subjectMap.value(ROpenSslTool::CertificateSubject::CommonName::key));
    this->commonNameEdit->setPlaceholderText(tr("Common name"));
    subjectLayout->addRow("CN:",this->commonNameEdit);

    QGroupBox *csrGroupBox = new QGroupBox(tr("Certificate signing request"));
    mainLayout->addWidget(csrGroupBox);

    QVBoxLayout *csrLayout = new QVBoxLayout;
    csrGroupBox->setLayout(csrLayout);

    this->csrBrowser = new RTextBrowser(false);
    csrLayout->addWidget(this->csrBrowser);

    QPushButton *generateCsrButton = new QPushButton(generateIcon, tr("Generate certificate signing request"));
    csrLayout->addWidget(generateCsrButton);

    QHBoxLayout *requestLayout = new QHBoxLayout;
    csrLayout->addLayout(requestLayout);

    this->directRequestButton = new QPushButton(directRequestIcon, tr("Direct request"));
    requestLayout->addWidget(this->directRequestButton);

    this->directRequestWithTokenButton = new QPushButton(directRequestIcon, tr("Direct request (with token)"));
    requestLayout->addWidget(this->directRequestWithTokenButton);

    this->emailRequestButton = new QPushButton(emailRequestIcon, tr("E-mail request"));
    requestLayout->addWidget(this->emailRequestButton);

    this->registerUserButton = new QPushButton(registerUserIcon, tr("Register new user"));
    csrLayout->addWidget(this->registerUserButton);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);

    this->onCsrTextChanged();

    QObject::connect(newPrivateKeyFileButton,&RFileChooserButton::fileNameChanged,this,&ROpenSslCsrDialog::onNewPrivateKeyFileNameChanged);
    QObject::connect(newPrivateKeyPasswordEdit,&QLineEdit::textChanged,this,&ROpenSslCsrDialog::onNewPrivateKeyPasswordChanged);
    QObject::connect(newCertificateFileButton,&RFileChooserButton::fileNameChanged,this,&ROpenSslCsrDialog::onNewCertificateFileNameChanged);
    QObject::connect(generateCsrButton,&QPushButton::clicked,this,&ROpenSslCsrDialog::onGenerateCsrClicked);
    QObject::connect(this->csrBrowser,&RTextBrowser::textChanged,this,&ROpenSslCsrDialog::onCsrTextChanged);
    QObject::connect(this->directRequestButton,&QPushButton::clicked,this,&ROpenSslCsrDialog::onDirectRequestClicked);
    QObject::connect(this->directRequestWithTokenButton,&QPushButton::clicked,this,&ROpenSslCsrDialog::onDirectRequestWithTokenClicked);
    QObject::connect(this->emailRequestButton,&QPushButton::clicked,this,&ROpenSslCsrDialog::onEmailRequestClicked);
    QObject::connect(this->registerUserButton,&QPushButton::clicked,this,&ROpenSslCsrDialog::onRegisterUserClicked);
}

QByteArray ROpenSslCsrDialog::loadKey(const QString &openSslToolPath,
                                      const QString &fileName,
                                      const QString &password)
{
    QString timeStamp(QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm:ss.zzz"));
    QString tmpFile = QDir::temp().absoluteFilePath(QString("key.%1.txt").arg(timeStamp));

    ROpenSslToolSettings openSslToolSettings;
    openSslToolSettings.setOpenSslPath(openSslToolPath);

    ROpenSslTool openSslTool(openSslToolSettings);

    try
    {
        openSslTool.outputKeyAsText(fileName,password,tmpFile);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to generate text output of private key using OpenSSL tool. %s\n", error.getMessage().toUtf8().constData());
        return QByteArray();
    }

    QByteArray content;

    QFile keyTxtFile(tmpFile);
    if (!keyTxtFile.open(QIODevice::ReadOnly))
    {
        RLogger::warning("Couldn't open SSL txt file \"%s\" for reading.\n", tmpFile.toUtf8().constData());
        return QByteArray();
    }

    content = keyTxtFile.readAll();

    keyTxtFile.close();

    if (!QFile::remove(tmpFile))
    {
        RLogger::warning("Failed to remove file \"%s\".\n", tmpFile.toUtf8().constData());
    }

    return content;
}

QByteArray ROpenSslCsrDialog::loadCsr(const QString &fileName)
{
    QByteArray content;

    QFile csrFile(fileName);
    if (!csrFile.open(QIODevice::ReadOnly))
    {
        RLogger::warning("Couldn't open SSL csr file \"%s\" for reading.\n", fileName.toUtf8().constData());
        return QByteArray();
    }

    content = csrFile.readAll();

    csrFile.close();

    return content;
}

QByteArray ROpenSslCsrDialog::loadCertificate(const QString &openSslToolPath, const QString &fileName)
{
    QString timeStamp(QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm:ss.zzz"));
    QString tmpFile = QDir::temp().absoluteFilePath(QString("cert.%1.txt").arg(timeStamp));

    ROpenSslToolSettings openSslToolSettings;
    openSslToolSettings.setOpenSslPath(openSslToolPath);

    ROpenSslTool openSslTool(openSslToolSettings);

    try
    {
        openSslTool.outputCertificateAsText(fileName,tmpFile);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to generate text output of certificate using OpenSSL tool. %s\n", error.getMessage().toUtf8().constData());
        return QByteArray();
    }

    QByteArray content;

    QFile certificateTxtFile(tmpFile);
    if (!certificateTxtFile.open(QIODevice::ReadOnly))
    {
        RLogger::warning("Couldn't open SSL txt file \"%s\" for reading.\n", tmpFile.toUtf8().constData());
        return QByteArray();
    }

    content = certificateTxtFile.readAll();

    certificateTxtFile.close();

    if (!QFile::remove(tmpFile))
    {
        RLogger::warning("Failed to remove file \"%s\".\n", tmpFile.toUtf8().constData());
    }

    return content;
}

void ROpenSslCsrDialog::onNewPrivateKeyFileNameChanged(const QString &fileName)
{
    this->newKeyPath = fileName;
}

void ROpenSslCsrDialog::onNewPrivateKeyPasswordChanged(const QString &password)
{
    this->newKeyPassword = password;
}

void ROpenSslCsrDialog::onNewCertificateFileNameChanged(const QString &fileName)
{
    this->newCertificatePath = fileName;
}

void ROpenSslCsrDialog::onGenerateCsrClicked()
{
    QStringList emptySubjectFields;
    if (this->territoryCombo->currentTerritoryCode().isEmpty())
    {
        emptySubjectFields.append(QString("%1 (%2)").arg(ROpenSslTool::CertificateSubject::Country::key,
                                                         ROpenSslTool::CertificateSubject::Country::name));
    }
    if (this->stateEdit->text().isEmpty())
    {
        emptySubjectFields.append(QString("%1 (%2)").arg(ROpenSslTool::CertificateSubject::State::key,
                                                         ROpenSslTool::CertificateSubject::State::name));
    }
    if (this->locationEdit->text().isEmpty())
    {
        emptySubjectFields.append(QString("%1 (%2)").arg(ROpenSslTool::CertificateSubject::Location::key,
                                                         ROpenSslTool::CertificateSubject::Location::name));
    }
    if (this->organizationEdit->text().isEmpty())
    {
        emptySubjectFields.append(QString("%1 (%2)").arg(ROpenSslTool::CertificateSubject::Organization::key,
                                                         ROpenSslTool::CertificateSubject::Organization::name));
    }
    if (this->organizationUnitEdit->text().isEmpty())
    {
        emptySubjectFields.append(QString("%1 (%2)").arg(ROpenSslTool::CertificateSubject::OrganizationUnit::key,
                                                         ROpenSslTool::CertificateSubject::OrganizationUnit::name));
    }
    if (this->commonNameEdit->text().isEmpty())
    {
        emptySubjectFields.append(QString("%1 (%2)").arg(ROpenSslTool::CertificateSubject::CommonName::key,
                                                         ROpenSslTool::CertificateSubject::CommonName::name));
    }
    if (!emptySubjectFields.isEmpty())
    {
        QString errorMessage = tr("Following fields must not be empty") + "<ul>";
        for (const QString &emptyField : emptySubjectFields)
        {
            errorMessage += "<li>" + emptyField + "</li>";
        }
        errorMessage += "</ul>";
        RMessageBox::critical(this,tr("No value specified"),errorMessage);
        return;
    }

    QMap<QString,QString> newSubjectMap;
    newSubjectMap.insert(ROpenSslTool::CertificateSubject::Country::key,this->territoryCombo->currentTerritoryCode());
    newSubjectMap.insert(ROpenSslTool::CertificateSubject::State::key,this->stateEdit->text());
    newSubjectMap.insert(ROpenSslTool::CertificateSubject::Location::key,this->locationEdit->text());
    newSubjectMap.insert(ROpenSslTool::CertificateSubject::Organization::key,this->organizationEdit->text());
    newSubjectMap.insert(ROpenSslTool::CertificateSubject::OrganizationUnit::key,this->organizationUnitEdit->text());
    newSubjectMap.insert(ROpenSslTool::CertificateSubject::CommonName::key,this->commonNameEdit->text());

    ROpenSslToolSettings openSslToolSettings;
    openSslToolSettings.setOpenSslPath(this->openSslToolPath);
    openSslToolSettings.setOpenSslCnfPath(this->openSslCnfPath);

    ROpenSslTool openSslTool(openSslToolSettings);

    try
    {
        openSslTool.generateKey(this->newKeyPath,this->newKeyPassword);
        openSslTool.generateCsr(this->newKeyPath,this->newKeyPassword,newSubjectMap,this->newCsrPath);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to generate private key and CSR using OpenSSL tool. %s\n", error.getMessage().toUtf8().constData());
        QString errorMessage = tr("Failed to generate private key and CSR using OpenSSL tool.")
                             + "<br/>"
                               + error.getMessage();
        RMessageBox::critical(this,tr("CSR generation failed"),errorMessage);
        return;
    }

    QByteArray csr(ROpenSslCsrDialog::loadCsr(this->newCsrPath));
    this->csrBrowser->setText(csr.constData());
}

void ROpenSslCsrDialog::onCsrTextChanged()
{
    bool csrIsEmpty = this->csrBrowser->document()->isEmpty();
    this->directRequestButton->setDisabled(csrIsEmpty);
    this->directRequestWithTokenButton->setDisabled(csrIsEmpty);
    this->emailRequestButton->setDisabled(csrIsEmpty);
    this->registerUserButton->setDisabled(csrIsEmpty);
}

void ROpenSslCsrDialog::onDirectRequestClicked()
{
    this->privateCloudClient->requestCsrProcess(this->csrBrowser->toPlainText().toUtf8().toBase64());
}

void ROpenSslCsrDialog::onDirectRequestWithTokenClicked()
{
    QString authUser = this->commonNameEdit->text();
    QString authToken = QInputDialog::getText(this,tr("Authentication token"),tr("Token") + ":");
    if (authToken.isEmpty())
    {
        RMessageBox::warning(this,tr("Authentication token not provided"),tr("No authentication token was provided, therefore no request will be sent."));
    }
    else
    {
        this->publicCloudClient->requestCsrProcess(this->csrBrowser->toPlainText().toUtf8().toBase64(),authUser,authToken);
    }
}

void ROpenSslCsrDialog::onEmailRequestClicked()
{
    QDesktopServices::openUrl(QUrl(QString("mailto:?to=csr@range-software.com&subject=Certification Signing Request&body=%1").arg(this->csrBrowser->toPlainText()), QUrl::TolerantMode));
}

void ROpenSslCsrDialog::onRegisterUserClicked()
{
    this->publicCloudClient->requestUserRegister(this->commonNameEdit->text());
}

void ROpenSslCsrDialog::onSignedCertificate(QSslCertificate certificate)
{
    if (!RFileTools::writeBinaryFile(this->newCertificatePath,certificate.toPem()))
    {
        RLogger::error("Failed to write certificate file \'%s\'.\n",this->newCertificatePath.toUtf8().constData());
        return;
    }

    QString questionMessage = tr("Do you want to apply newly generated key and signed certificate?")
                              + "<br/>"
                              + "<ul>"
                              + "<li><b>" + tr("Private key") + ":</b> <tt>" + this->newKeyPath + "</tt></li>"
                              + "<li><b>" + tr("Password") + ":</b> <tt>********</tt></li>"
                              + "<li><b>" + tr("Certificate") + ":</b> <tt>" + this->newCertificatePath + "</tt></li>"
                              + "</ul>";

    int retVal = RMessageBox::question(this,tr("Signed certificate"),questionMessage);
    if (retVal == RMessageBox::Yes)
    {
        emit this->identityAvailable(this->newKeyPath,this->newKeyPassword,this->newCertificatePath);
    }
}

void ROpenSslCsrDialog::onUserRegistered(std::tuple<RUserInfo, QList<RAuthToken> > registrationInfo)
{
    const RUserInfo &userInfo = std::get<0>(registrationInfo);
    const QList<RAuthToken> &authTokens = std::get<1>(registrationInfo);

    if (authTokens.empty())
    {
        RMessageBox::critical(this,tr("No tokens received"),tr("No authentication token was received."));
        return;
    }

    QString noticeMessage = tr("New user was successfully registered.")
                          + "<ul>"
                          + "<li><b>" + tr("User name") + ":</b> <tt>" + userInfo.getName() + "</tt></li>"
                          + "<li><b>" + tr("User groups") + ":</b> <tt>" + userInfo.getGroupNames().join(", ") + "</tt></li>"
                          + "<li><b>" + tr("Authentication token") + ":</b> <tt>" + authTokens.at(0).getContent() + "</tt></li>"
                          + "</ul>"
                          + tr("Certificate signing request is ready to be sent.");
    RMessageBox::information(this,tr("User registered"),noticeMessage);

    this->publicCloudClient->requestCsrProcess(this->csrBrowser->toPlainText().toUtf8().toBase64(),userInfo.getName(),authTokens.at(0).getContent());
}
