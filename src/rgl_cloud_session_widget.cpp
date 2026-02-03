#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QFileInfo>
#include <QGroupBox>

#include <rbl_error.h>
#include <rbl_logger.h>

#include <rcl_open_ssl_tool.h>

#include "rgl_application_settings.h"
#include "rgl_cloud_session_widget.h"
#include "rgl_message_box.h"
#include "rgl_open_ssl_csr_dialog.h"
#include "rgl_text_browser_dialog.h"

RCloudSessionWidget::RCloudSessionWidget(const RCloudSessionInfo &sessionInfo,
                                         RCloudConnectionHandler *connectionHandler,
                                         RApplicationSettings *applicationSettings,
                                         QWidget *parent)
    : QWidget{parent}
    , sessionInfo{sessionInfo}
    , connectionHandler{connectionHandler}
    , applicationSettings{applicationSettings}
{
    this->publicCloudClient = this->connectionHandler->createPublicClient(this);
    this->privateCloudClient = this->connectionHandler->createPrivateClient(this);

    QObject::connect(this->publicCloudClient,&RCloudClient::connectionTestFinished,this,&RCloudSessionWidget::onPublicConnectionTestFinished);
    QObject::connect(this->privateCloudClient,&RCloudClient::connectionTestFinished,this,&RCloudSessionWidget::onPrivateConnectionTestFinished);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *nameLayout = new QFormLayout;
    mainLayout->addLayout(nameLayout);

    this->nameEdit = new QLineEdit(this->sessionInfo.getName());
    this->nameEdit->setReadOnly(true);
    this->nameEdit->setPlaceholderText(tr("Session descriptive name"));
    // nameEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[a-zA-Z0-9._ ]{1,32}$")));
    nameLayout->addRow(tr("Name") + ":",this->nameEdit);

    QGroupBox *hostGroup = new QGroupBox(tr("Cloud host"));
    mainLayout->addWidget(hostGroup);

    QFormLayout *hostLayout = new QFormLayout;
    hostGroup->setLayout(hostLayout);

    this->hostNameEdit = new QLineEdit(this->sessionInfo.getHostName());
    this->hostNameEdit->setPlaceholderText(tr("Host address"));
    hostLayout->addRow(tr("Host") + ":",this->hostNameEdit);

    this->publicPortSpin = new QSpinBox;
    this->publicPortSpin->setRange(1,49151);
    this->publicPortSpin->setValue(this->sessionInfo.getPublicPort());
    hostLayout->addRow(tr("Public port") + ":", this->publicPortSpin);

    this->privatePortSpin = new QSpinBox;
    this->privatePortSpin->setRange(1,49151);
    this->privatePortSpin->setValue(this->sessionInfo.getPrivatePort());
    hostLayout->addRow(tr("Private port") + ":", this->privatePortSpin);

    this->timeoutSpin = new QSpinBox;
    this->timeoutSpin->setRange(0,INT_MAX);
    this->timeoutSpin->setValue(this->sessionInfo.getTimeout());
    this->timeoutSpin->setToolTip(tr("Connection timeout in milliseconds"));
    hostLayout->addRow(tr("Timeout") + ":", this->timeoutSpin);

    QHBoxLayout *hostCertificateLayout = new QHBoxLayout;
    hostLayout->addRow(tr("Certificate") + ":", hostCertificateLayout);

    this->hostCertificateFileButton = new RFileChooserButton(QString(),RFileChooserButton::OpenFile);
    this->hostCertificateFileButton->setFileName(this->sessionInfo.getHostTrustStore().getCertificateFile());
    this->hostCertificateFileButton->setSearchDirectory(RApplicationSettings::getCertDir());
    hostCertificateLayout->addWidget(this->hostCertificateFileButton);

    QPushButton *hostCertificateViewButton = new QPushButton(tr("View"));
    hostCertificateLayout->addWidget(hostCertificateViewButton);

    QPushButton *testPublicRequestButton = new QPushButton(tr("Send test request to public Cloud"));
    hostLayout->addRow(testPublicRequestButton);

    QPushButton *testPrivateRequestButton = new QPushButton(tr("Send test request to private Cloud"));
    hostLayout->addRow(testPrivateRequestButton);

    QGroupBox *clientGroup = new QGroupBox(tr("Cloud client"));
    mainLayout->addWidget(clientGroup);

    QFormLayout *clientLayout = new QFormLayout;
    clientGroup->setLayout(clientLayout);

    QHBoxLayout *clientPrivateKeyLayout = new QHBoxLayout;
    clientLayout->addRow(tr("Private key") + ":",clientPrivateKeyLayout);

    this->clientPrivateKeyFileButton = new RFileChooserButton(QString(),RFileChooserButton::OpenFile);
    this->clientPrivateKeyFileButton->setFileName(this->sessionInfo.getClientKeyStore().getKeyFile());
    this->clientPrivateKeyFileButton->setSearchDirectory(RApplicationSettings::getCertDir());
    clientPrivateKeyLayout->addWidget(this->clientPrivateKeyFileButton);

    QPushButton *clientPrivateKeyViewButton = new QPushButton(tr("View"));
    clientPrivateKeyLayout->addWidget(clientPrivateKeyViewButton);

    this->clientPrivateKeyPasswordEdit = new QLineEdit(this->sessionInfo.getClientKeyStore().getPassword());
    this->clientPrivateKeyPasswordEdit->setEchoMode(QLineEdit::Password);
    clientLayout->addRow(tr("Private key password") + ":",this->clientPrivateKeyPasswordEdit);

    QHBoxLayout *clientCertificateLayout = new QHBoxLayout;
    clientLayout->addRow(tr("Certificate") + ":",clientCertificateLayout);

    this->clientCertificateFileButton = new RFileChooserButton(QString(),RFileChooserButton::OpenFile);
    this->clientCertificateFileButton->setFileName(this->sessionInfo.getClientKeyStore().getCertificateFile());
    this->clientCertificateFileButton->setSearchDirectory(RApplicationSettings::getCertDir());
    clientCertificateLayout->addWidget(this->clientCertificateFileButton);

    QPushButton *clientCertificateViewButton = new QPushButton(tr("View"));
    clientCertificateLayout->addWidget(clientCertificateViewButton);

    QPushButton *clientCertificateRequestButton = new QPushButton(tr("Request new certificate"));
    clientLayout->addRow(clientCertificateRequestButton);

    QObject::connect(this->nameEdit,&QLineEdit::textChanged,this,&RCloudSessionWidget::onNameChanged);
    QObject::connect(this->hostNameEdit,&QLineEdit::textChanged,this,&RCloudSessionWidget::onHostNameChanged);
    QObject::connect(this->publicPortSpin,&QSpinBox::valueChanged,this,&RCloudSessionWidget::onPublicPortChanged);
    QObject::connect(this->privatePortSpin,&QSpinBox::valueChanged,this,&RCloudSessionWidget::onPrivatePortChanged);
    QObject::connect(this->timeoutSpin,&QSpinBox::valueChanged,this,&RCloudSessionWidget::onTimeoutChanged);
    QObject::connect(this->hostCertificateFileButton,&RFileChooserButton::fileNameChanged,this,&RCloudSessionWidget::onHostCertificateFileChanged);
    QObject::connect(this->clientPrivateKeyFileButton,&RFileChooserButton::fileNameChanged,this,&RCloudSessionWidget::onClientPrivateKeyFileChanged);
    QObject::connect(this->clientPrivateKeyPasswordEdit,&QLineEdit::textChanged,this,&RCloudSessionWidget::onClientPrivateKeyPasswordChanged);
    QObject::connect(this->clientCertificateFileButton,&RFileChooserButton::fileNameChanged,this,&RCloudSessionWidget::onClientCertificateFileChanged);
    QObject::connect(testPublicRequestButton,&QPushButton::clicked,this,&RCloudSessionWidget::onTestPublicRequestButtonClicked);
    QObject::connect(testPrivateRequestButton,&QPushButton::clicked,this,&RCloudSessionWidget::onTestPrivateRequestButtonClicked);

    QObject::connect(clientCertificateRequestButton,&QPushButton::clicked,this,&RCloudSessionWidget::onClientCertificateRequestClicked);
    QObject::connect(clientPrivateKeyViewButton,&QPushButton::clicked,this,&RCloudSessionWidget::onClientPrivateKeyViewClicked);
    QObject::connect(hostCertificateViewButton,&QPushButton::clicked,this,&RCloudSessionWidget::onHostCertificateViewClicked);
    QObject::connect(clientCertificateViewButton,&QPushButton::clicked,this,&RCloudSessionWidget::onClientCertificateViewClicked);
}

const RCloudSessionInfo &RCloudSessionWidget::getSessionInfo() const
{
    return this->sessionInfo;
}

void RCloudSessionWidget::setSessionInfo(const RCloudSessionInfo &sessionInfo)
{
    this->setDisabled(sessionInfo.getName().isEmpty());

    bool signalsBlockedSaved = this->signalsBlocked();
    this->blockSignals(true);

    this->nameEdit->setText(sessionInfo.getName());
    this->hostNameEdit->setText(sessionInfo.getHostName());
    this->publicPortSpin->setValue(sessionInfo.getPublicPort());
    this->privatePortSpin->setValue(sessionInfo.getPrivatePort());
    this->timeoutSpin->setValue(sessionInfo.getTimeout());
    this->hostCertificateFileButton->setFileName(sessionInfo.getHostTrustStore().getCertificateFile());
    this->clientPrivateKeyFileButton->setFileName(sessionInfo.getClientKeyStore().getKeyFile());
    this->clientPrivateKeyPasswordEdit->setText(sessionInfo.getClientKeyStore().getPassword());
    this->clientCertificateFileButton->setFileName(sessionInfo.getClientKeyStore().getCertificateFile());

    this->blockSignals(signalsBlockedSaved);
}

void RCloudSessionWidget::onNameChanged(const QString &hostName)
{
    this->sessionInfo.setName(hostName);
}

void RCloudSessionWidget::onHostNameChanged(const QString &hostName)
{
    this->sessionInfo.setHostName(hostName);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onPublicPortChanged(int publicPort)
{
    this->sessionInfo.setPublicPort(publicPort);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onPrivatePortChanged(int privatePort)
{
    this->sessionInfo.setPrivatePort(privatePort);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onTimeoutChanged(int timeout)
{
    this->sessionInfo.setTimeout(timeout);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onHostCertificateFileChanged(const QString &fileName)
{
    RTlsTrustStore trustStore = this->sessionInfo.getHostTrustStore();
    trustStore.setCertificateFile(fileName);
    this->sessionInfo.setHostTrustStore(trustStore);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onClientPrivateKeyFileChanged(const QString &fileName)
{
    RTlsKeyStore keyStore = this->sessionInfo.getClientKeyStore();
    keyStore.setKeyFile(fileName);
    this->sessionInfo.setClientKeyStore(keyStore);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onClientPrivateKeyPasswordChanged(const QString &password)
{
    RTlsKeyStore keyStore = this->sessionInfo.getClientKeyStore();
    keyStore.setPassword(password);
    this->sessionInfo.setClientKeyStore(keyStore);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onClientCertificateFileChanged(const QString &fileName)
{
    RTlsKeyStore keyStore = this->sessionInfo.getClientKeyStore();
    keyStore.setCertificateFile(fileName);
    this->sessionInfo.setClientKeyStore(keyStore);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::onTestPublicRequestButtonClicked()
{
    try
    {
        this->publicCloudClient->setHttpClientSettings(this->connectionHandler->createPublicHttpClientSettings(this->sessionInfo));
        QString publicCloudUrl = RHttpClient::buildUrl(this->sessionInfo.getHostName(),this->sessionInfo.getPublicPort());
        this->publicCloudClient->requestTest(tr("Public Cloud is reachable.") + " (" + publicCloudUrl  + ")");
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request public cloud connection test. %s\n",rError.getMessage().toUtf8().constData());
    }
}

void RCloudSessionWidget::onTestPrivateRequestButtonClicked()
{
    try
    {
        this->privateCloudClient->setHttpClientSettings(this->connectionHandler->createPrivateHttpClientSettings(this->sessionInfo));
        QString privateCloudUrl = RHttpClient::buildUrl(this->sessionInfo.getHostName(),this->sessionInfo.getPrivatePort());
        this->privateCloudClient->requestTest(tr("Private Cloud is reachable.") + " (" + privateCloudUrl  + ")");
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request private cloud connection test. %s\n",rError.getMessage().toUtf8().constData());
    }
}

void RCloudSessionWidget::onPublicConnectionTestFinished(QString message)
{
    RMessageBox::information(this,tr("Public Cloud connection test"),message);
}

void RCloudSessionWidget::onPrivateConnectionTestFinished(QString message)
{
    RMessageBox::information(this,tr("Private Cloud connection test"),message);
}

void RCloudSessionWidget::onClientCertificateRequestClicked()
{
    QMap<QString,QString> subjectMap = RTlsTrustStore::findSubjectFields(this->sessionInfo.getClientKeyStore().getCertificateFile());

    if (subjectMap.isEmpty())
    {
        subjectMap.insert(ROpenSslTool::CertificateSubject::Country::key,QLocale::territoryToCode(QLocale::system().territory()));
        subjectMap.insert(ROpenSslTool::CertificateSubject::CommonName::key,this->applicationSettings->getUserEmail());
        subjectMap.insert(ROpenSslTool::CertificateSubject::Organization::key,"Range Software");
        subjectMap.insert(ROpenSslTool::CertificateSubject::OrganizationUnit::key,"Cloud");
    }

    QDateTime currentDateTime = QDateTime::currentDateTime();

    QString newPrivateKeyPath = RApplicationSettings::buildFilePathWithTimestamp(RApplicationSettings::getDefaultPrivateKeyPath(),currentDateTime);
    QString newCsrPath = RApplicationSettings::buildFilePathWithTimestamp(RApplicationSettings::getDefaultCsrPath(),currentDateTime);
    QString newCertificatePath = RApplicationSettings::buildFilePathWithTimestamp(RApplicationSettings::getDefaultCertificatePath(),currentDateTime);

    ROpenSslCsrDialog *openSslCsrDialog = new ROpenSslCsrDialog(this->connectionHandler,
                                                                this->applicationSettings->getOpensslToolPath(),
                                                                this->applicationSettings->getOpensslConfPath(),
                                                                newPrivateKeyPath,
                                                                this->clientPrivateKeyPasswordEdit->text(),
                                                                newCsrPath,
                                                                newCertificatePath,
                                                                subjectMap,
                                                                this);

    QObject::connect(openSslCsrDialog,&ROpenSslCsrDialog::identityAvailable,this,&RCloudSessionWidget::onIdentityAvailable);

    openSslCsrDialog->exec();
}

void RCloudSessionWidget::onClientPrivateKeyViewClicked()
{
    QByteArray keyContent = ROpenSslCsrDialog::loadKey(this->applicationSettings->getOpensslToolPath(),
                                                       this->clientPrivateKeyFileButton->getFileName(),
                                                       this->clientPrivateKeyPasswordEdit->text());
    RTextBrowserDialog *keyContentDialog = new RTextBrowserDialog(tr("Private key"),
                                                                this->clientPrivateKeyFileButton->getFileName(),
                                                                QString(keyContent));
    keyContentDialog->exec();
}

void RCloudSessionWidget::onHostCertificateViewClicked()
{
    RCloudSessionWidget::viewCertificates(this->hostCertificateFileButton->getFileName());
}

void RCloudSessionWidget::onClientCertificateViewClicked()
{
    RCloudSessionWidget::viewCertificates(this->clientCertificateFileButton->getFileName());
}

void RCloudSessionWidget::onIdentityAvailable(const QString &keyPath, const QString &keyPassword, const QString &certificatePath)
{
    RLogger::info("Applying new identity (private key: \"%s\", certificate: \"%s\")\n",
                  keyPath.toUtf8().constData(),
                  certificatePath.toUtf8().constData());

    bool clientPrivateKeyFileButtonSignals = this->clientPrivateKeyFileButton->signalsBlocked();
    bool clientPrivateKeyPasswordEditSignals = this->clientPrivateKeyPasswordEdit->signalsBlocked();
    bool clientCertificateFileButtonSignals = this->clientCertificateFileButton->signalsBlocked();

    this->clientPrivateKeyFileButton->blockSignals(true);
    this->clientPrivateKeyPasswordEdit->blockSignals(true);
    this->clientCertificateFileButton->blockSignals(true);

    this->clientPrivateKeyFileButton->setFileName(keyPath);
    this->clientPrivateKeyPasswordEdit->setText(keyPassword);
    this->clientCertificateFileButton->setFileName(certificatePath);

    this->clientPrivateKeyFileButton->blockSignals(clientPrivateKeyFileButtonSignals);
    this->clientPrivateKeyPasswordEdit->blockSignals(clientPrivateKeyPasswordEditSignals);
    this->clientCertificateFileButton->blockSignals(clientCertificateFileButtonSignals);
    emit this->sessionInfoChanged(this->sessionInfo);
}

void RCloudSessionWidget::viewCertificates(const QString &certificatesPath)
{
    QList<QSslCertificate> clientCertificates = QSslCertificate::fromPath(certificatesPath,QSsl::EncodingFormat::Pem);
    if (clientCertificates.empty())
    {
        RLogger::error("Couldn't retrieve SSL client certificate from file \"%s\".",
                       certificatesPath.toUtf8().constData());
        return;
    }
    QString certificatesText;
    for (QSslCertificate &certificate : clientCertificates)
    {
        if (!certificatesText.isEmpty())
        {
            certificatesText += '\n';
        }
        certificatesText += RTlsTrustStore::toText(certificate);
    }

    RTextBrowserDialog *certificateContentDialog = new RTextBrowserDialog(tr("Certificate"),certificatesPath,certificatesText);
    certificateContentDialog->exec();
}
