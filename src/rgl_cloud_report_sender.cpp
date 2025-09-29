#include <rbl_logger.h>

#include <rcl_cloud_session_manager.h>
#include <rcl_report_record.h>

#include "rgl_cloud_report_sender.h"

RCloudReportSender::RCloudReportSender(const RApplicationSettings *applicationSettings, QObject *parent)
    : QObject{parent}
    , autodelete{false}
{
    RCloudSessionInfo rangeSession = RCloudSessionManager::getDefaultRangeSession();

    RHttpClientSettings httpClientSettings;
    httpClientSettings.setUrl(RHttpClient::buildUrl(rangeSession.getHostName(),rangeSession.getPublicPort()));
    httpClientSettings.setTimeout(rangeSession.getTimeout());
    httpClientSettings.setTlsTrustStore(rangeSession.getHostCertificate());
    httpClientSettings.setProxySettings(applicationSettings->getProxySettings());

    this->cloudClient = new RCloudClient(RHttpClient::Public,httpClientSettings,this);
    this->cloudClient->setBlocking(false);

    QObject::connect(this->cloudClient,&RCloudClient::reportSubmitted,this,&RCloudReportSender::onSubmitReportFinished);
    QObject::connect(this->cloudClient,&RCloudClient::actionFailed,this,&RCloudReportSender::onCloudActionFailed);
}

void RCloudReportSender::sendReport(const RReportRecord &reportRecord)
{
    this->cloudClient->requestSubmitReport(reportRecord);
}

void RCloudReportSender::sendReport(const RApplicationSettings *applicationSettings, const RReportRecord &reportRecord)
{
    RCloudReportSender *recordSender = new RCloudReportSender(applicationSettings,nullptr);
    recordSender->autodelete = true;
    recordSender->sendReport(reportRecord);
}

void RCloudReportSender::onSubmitReportFinished(QString message)
{
    RLogger::info("The report was successfully sent (size: %d).\n",message.length());
    emit this->reportSent(message);
    if (this->autodelete)
    {
        this->deleteLater();
    }
}

void RCloudReportSender::onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    RLogger::error("Failed to send the report (size: %d). %s\n",message.length(),errorMessage.toUtf8().constData());
    emit this->reportFailed(errorType,errorMessage,message);
    if (this->autodelete)
    {
        this->deleteLater();
    }
}
