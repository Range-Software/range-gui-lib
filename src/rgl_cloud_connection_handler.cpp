#include <rbl_logger.h>

#include "rgl_cloud_connection_handler.h"
#include "rgl_message_box.h"

const qint64 RCloudConnectionHandler::validPeriod = 60; // Seconds

RCloudConnectionHandler::RCloudConnectionHandler(const RCloudSessionManager *session, const RApplicationSettings *applicationSettings, QObject *parent)
    : QObject{parent}
    , messageBoxParentWidget{nullptr}
{
    this->lastPublicError.first = 0;
    this->lastPrivateError.first = 0;

    this->sessionInfo = session->findSession(session->findActiveSessionName());
    this->proxySettings = applicationSettings->getProxySettings();

    QObject::connect(session,&RCloudSessionManager::activeSessionChanged,this,&RCloudConnectionHandler::onActiveSessionChanged);
    QObject::connect(applicationSettings,&RApplicationSettings::proxySettingsChanged,this,&RCloudConnectionHandler::onProxySettingsChanged);
}

RCloudClient *RCloudConnectionHandler::createPublicClient(QObject *parent)
{
    RCloudClient *client = new RCloudClient(RHttpClient::Public,RCloudConnectionHandler::createPublicHttpClientSettings(this->sessionInfo,this->proxySettings),parent);

    QObject::connect(this,
                     &RCloudConnectionHandler::publicHttpClientSettingsChanged,
                     client,
                     &RCloudClient::setHttpClientSettings);
    QObject::connect(client,
                     &RCloudClient::actionFinished,
                     this,
                     &RCloudConnectionHandler::onPublicCloudActionFinished);
    QObject::connect(client,
                     &RCloudClient::actionFailed,
                     this,
                     &RCloudConnectionHandler::onPublicCloudActionFailed);

    return client;
}

RCloudClient *RCloudConnectionHandler::createPrivateClient(QObject *parent)
{
    RCloudClient *client = new RCloudClient(RHttpClient::Private,RCloudConnectionHandler::createPrivateHttpClientSettings(this->sessionInfo,this->proxySettings),parent);

    QObject::connect(this,
                     &RCloudConnectionHandler::privateHttpClientSettingsChanged,
                     client,
                     &RCloudClient::setHttpClientSettings);
    QObject::connect(client,
                     &RCloudClient::actionFinished,
                     this,
                     &RCloudConnectionHandler::onPrivateCloudActionFinished);
    QObject::connect(client,
                     &RCloudClient::actionFailed,
                     this,
                     &RCloudConnectionHandler::onPrivateCloudActionFailed);

    return client;
}

void RCloudConnectionHandler::setMessageBoxParentWidget(QWidget *widget)
{
    this->messageBoxParentWidget = widget;
}

RHttpClientSettings RCloudConnectionHandler::createPublicHttpClientSettings(const RCloudSessionInfo &sessionInfo) const
{
    return RCloudConnectionHandler::createPublicHttpClientSettings(sessionInfo,this->proxySettings);
}

RHttpClientSettings RCloudConnectionHandler::createPrivateHttpClientSettings(const RCloudSessionInfo &sessionInfo) const
{
    return RCloudConnectionHandler::createPrivateHttpClientSettings(sessionInfo,this->proxySettings);
}

RHttpClientSettings RCloudConnectionHandler::createPublicHttpClientSettings(const RCloudSessionInfo &sessionInfo, const RHttpProxySettings &proxySettings)
{
    RHttpClientSettings publicHttpClientSettings;
    publicHttpClientSettings.setUrl(RHttpClient::buildUrl(sessionInfo.getHostName(),sessionInfo.getPublicPort()));
    publicHttpClientSettings.setTimeout(sessionInfo.getTimeout());
    publicHttpClientSettings.setProxySettings(proxySettings);
    publicHttpClientSettings.setTlsTrustStore(sessionInfo.getHostTrustStore());

    return publicHttpClientSettings;
}

RHttpClientSettings RCloudConnectionHandler::createPrivateHttpClientSettings(const RCloudSessionInfo &sessionInfo, const RHttpProxySettings &proxySettings)
{
    RHttpClientSettings privateHttpClientSettings;
    privateHttpClientSettings.setUrl(RHttpClient::buildUrl(sessionInfo.getHostName(),sessionInfo.getPrivatePort()));
    privateHttpClientSettings.setTimeout(sessionInfo.getTimeout());
    privateHttpClientSettings.setProxySettings(proxySettings);
    privateHttpClientSettings.setTlsKeyStore(sessionInfo.getClientKeyStore());
    privateHttpClientSettings.setTlsTrustStore(sessionInfo.getHostTrustStore());

    return privateHttpClientSettings;
}

void RCloudConnectionHandler::onProxySettingsChanged(RHttpProxySettings proxySettings)
{
    this->proxySettings = proxySettings;

    RHttpClientSettings publicSettings = RCloudConnectionHandler::createPublicHttpClientSettings(this->sessionInfo, this->proxySettings);
    RHttpClientSettings privateSettings = RCloudConnectionHandler::createPrivateHttpClientSettings(this->sessionInfo, this->proxySettings);

    emit this->publicHttpClientSettingsChanged(publicSettings);
    emit this->privateHttpClientSettingsChanged(privateSettings);
}

void RCloudConnectionHandler::onActiveSessionChanged(RCloudSessionInfo sessionInfo)
{
    this->sessionInfo = sessionInfo;

    RHttpClientSettings publicSettings = RCloudConnectionHandler::createPublicHttpClientSettings(this->sessionInfo, this->proxySettings);
    RHttpClientSettings privateSettings = RCloudConnectionHandler::createPrivateHttpClientSettings(this->sessionInfo, this->proxySettings);

    emit this->publicHttpClientSettingsChanged(publicSettings);
    emit this->privateHttpClientSettingsChanged(privateSettings);
}

void RCloudConnectionHandler::onPublicCloudActionFinished()
{
    this->lastPublicError.first = 0;
    this->lastPublicError.second.clear();
}

void RCloudConnectionHandler::onPublicCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    RLogger::error("Public Cloud action has failed. %s\n",message.toUtf8().constData());

    qint64 currentSecs = QDateTime::currentSecsSinceEpoch();

    bool showError = (currentSecs - this->lastPublicError.first) > RCloudConnectionHandler::validPeriod || this->lastPublicError.second != message;

    this->lastPublicError.first = currentSecs;
    this->lastPublicError.second = message;

    if (showError)
    {
        RMessageBox::critical(this->messageBoxParentWidget,tr("Public Cloud transfer failed"),message,errorType,errorMessage);
    }
}

void RCloudConnectionHandler::onPrivateCloudActionFinished()
{
    this->lastPrivateError.first = 0;
    this->lastPrivateError.second.clear();
}

void RCloudConnectionHandler::onPrivateCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    RLogger::error("Private Cloud action has failed. %s\n",message.toUtf8().constData());

    qint64 currentSecs = QDateTime::currentSecsSinceEpoch();

    bool showError = (currentSecs - this->lastPrivateError.first) > RCloudConnectionHandler::validPeriod || this->lastPrivateError.second != message;

    this->lastPrivateError.first = currentSecs;
    this->lastPrivateError.second = message;

    if (showError)
    {
        RMessageBox::critical(this->messageBoxParentWidget,tr("Private Cloud transfer failed"),message,errorType,errorMessage);
    }
}
