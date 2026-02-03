#include <QFile>

#include <rbl_error.h>
#include <rbl_logger.h>

#include <rcl_file_tools.h>
#include <rcl_open_ssl_tool.h>

#include "rgl_open_ssl_csr_task.h"
#include "rgl_message_box.h"

ROpenSslCsrTask::ROpenSslCsrTask(RApplicationSettings *applicationSettings,
                                 RCloudConnectionHandler *connectionHandler,
                                 const RCloudSessionInfo &sessionInfo,
                                 QWidget *parent)
    : QObject{parent}
    , sessionInfo{sessionInfo}
{
    ROpenSslToolSettings openSslToolSettings;
    openSslToolSettings.setOpenSslPath(applicationSettings->getOpensslToolPath());
    openSslToolSettings.setOpenSslCnfPath(applicationSettings->getOpensslConfPath());

    ROpenSslTool openSslTool(openSslToolSettings);

    QDateTime csrDateTime = QDateTime::currentDateTime();

    this->clientKeyStore.setKeyFile(RApplicationSettings::buildFilePathWithTimestamp(RApplicationSettings::getDefaultPrivateKeyPath(),csrDateTime));
    this->clientKeyStore.setPassword(sessionInfo.getClientKeyStore().getPassword());
    this->clientKeyStore.setCertificateFile(RApplicationSettings::buildFilePathWithTimestamp(RApplicationSettings::getDefaultCertificatePath(),csrDateTime));

    this->clientCsrFile = RApplicationSettings::buildFilePathWithTimestamp(RApplicationSettings::getDefaultCsrPath(),csrDateTime);
    QString oldClientCertificateFile = sessionInfo.getClientKeyStore().getCertificateFile();

    QByteArray csrContent;

    try
    {
        RLogger::info("Generating private key \"%s\"\n",this->clientKeyStore.getKeyFile().toUtf8().constData());
        openSslTool.generateKey(this->clientKeyStore.getKeyFile(),this->clientKeyStore.getPassword());
        RLogger::info("Generating CSR \"%s\"\n",this->clientCsrFile.toUtf8().constData());
        openSslTool.generateCsr(this->clientKeyStore.getKeyFile(),this->clientKeyStore.getPassword(),oldClientCertificateFile,this->clientCsrFile);

        QFile csrFile(this->clientCsrFile);
        if (!csrFile.open(QIODevice::ReadOnly))
        {
            throw RError(RError::ReadFile,R_ERROR_REF, "Couldn't open SSL csr file \"%s\" for reading.\n", this->clientCsrFile.toUtf8().constData());
        }

        RLogger::debug("Reading CSR from \"%s\"\n",this->clientCsrFile.toUtf8().constData());
        csrContent = csrFile.readAll();
        csrFile.close();
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to generate private key and CSR using OpenSSL tool. %s\n", error.getMessage().toUtf8().constData());
        QString errorMessage = tr("Failed to generate private key and CSR using OpenSSL tool.")
                               + "<br/>"
                               + error.getMessage();
        RMessageBox::critical(parent,tr("CSR generation failed"),errorMessage);
        return;
    }

    RLogger::debug("Create private Cloud client.\n");
    RCloudClient *privateCloudClient = connectionHandler->createPrivateClient(this);

    QObject::connect(privateCloudClient,&RCloudClient::signedCertificateAvailable,this,&ROpenSslCsrTask::onSignedCertificate);
    QObject::connect(privateCloudClient,&RCloudClient::actionFailed,this,&ROpenSslCsrTask::onActionFailed);

    RLogger::info("Requesting CSR process on private Cloud.\n");
    privateCloudClient->requestCsrProcess(csrContent.toBase64());
}

void ROpenSslCsrTask::onSignedCertificate(QSslCertificate certificate)
{
    RLogger::info("Writing certificate file \"%s\".\n",this->clientKeyStore.getCertificateFile().toUtf8().constData());
    if (!RFileTools::writeBinaryFile(this->clientKeyStore.getCertificateFile(),certificate.toPem()))
    {
        RLogger::error("Failed to write certificate file \'%s\'.\n",this->clientKeyStore.getCertificateFile().toUtf8().constData());
        return;
    }
    this->sessionInfo.setClientKeyStore(this->clientKeyStore);

    emit this->finished(this->sessionInfo);
}

void ROpenSslCsrTask::onActionFailed(RError::Type errorType, QString errorMessage, QString message)
{
    QString failedMessage = QString("%1 (%2)").arg(errorMessage,message);

    emit this->failed(failedMessage);
}
