#ifndef RGL_CLOUD_CONNECTION_HANDLER_H
#define RGL_CLOUD_CONNECTION_HANDLER_H

#include <QObject>
#include <QPair>

#include <rcl_http_client_settings.h>
#include <rcl_cloud_client.h>
#include <rcl_cloud_session_manager.h>

#include "rgl_application_settings.h"

class RCloudConnectionHandler : public QObject
{
    Q_OBJECT

    protected:

        static const qint64 validPeriod;

        RCloudSessionInfo sessionInfo;
        RHttpProxySettings proxySettings;

        QWidget *messageBoxParentWidget;

        QPair<qint64,QString> lastPublicError;
        QPair<qint64,QString> lastPrivateError;

    public:

        //! Constructor.
        explicit RCloudConnectionHandler(const RCloudSessionManager *session, const RApplicationSettings *applicationSettings, QObject *parent);

        //! Create public client.
        RCloudClient *createPublicClient(QObject *parent);

        //! Create private client.
        RCloudClient *createPrivateClient(QObject *parent);

        //! Set message-box parent widget.
        void setMessageBoxParentWidget(QWidget *widget);

        //! Create public http client settings.
        RHttpClientSettings createPublicHttpClientSettings(const RCloudSessionInfo &sessionInfo) const;

        //! Create private http client settings.
        RHttpClientSettings createPrivateHttpClientSettings(const RCloudSessionInfo &sessionInfo) const;

    private:

        //! Create public http client settings.
        static RHttpClientSettings createPublicHttpClientSettings(const RCloudSessionInfo &sessionInfo, const RHttpProxySettings &proxySettings);

        //! Create private http client settings.
        static RHttpClientSettings createPrivateHttpClientSettings(const RCloudSessionInfo &sessionInfo, const RHttpProxySettings &proxySettings);

    protected slots:

        //! Proxy settings has changed.
        void onProxySettingsChanged(RHttpProxySettings proxySettings);

        //! Active session changed.
        void onActiveSessionChanged(RCloudSessionInfo sessionInfo);

        //! Public cloud action has finished.
        void onPublicCloudActionFinished();

        //! Public cloud action has failed.
        void onPublicCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

        //! Private cloud action has finished.
        void onPrivateCloudActionFinished();

        //! Private cloud action has failed.
        void onPrivateCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

    signals:

        //! Public http client settings changed.
        void publicHttpClientSettingsChanged(const RHttpClientSettings &clientSettings);

        //! Private http client settings changed.
        void privateHttpClientSettingsChanged(const RHttpClientSettings &clientSettings);

};

#endif // RGL_CLOUD_CONNECTION_HANDLER_H
