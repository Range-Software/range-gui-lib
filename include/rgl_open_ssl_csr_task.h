#ifndef RGL_OPEN_SSL_CSR_TASK_H
#define RGL_OPEN_SSL_CSR_TASK_H

#include <QObject>

#include <rcl_cloud_session_manager.h>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"

class ROpenSslCsrTask : public QObject
{

    Q_OBJECT

    protected:

        RCloudSessionInfo sessionInfo;
        RTlsKeyStore clientKeyStore;
        QString clientCsrFile;

    public:

        //! Constructor.
        explicit ROpenSslCsrTask(RApplicationSettings *applicationSettings,
                                 RCloudConnectionHandler *connectionHandler,
                                 const RCloudSessionInfo &sessionInfo,
                                 QWidget *parent = nullptr);

    protected slots:

        //! Certificate has been signed.
        void onSignedCertificate(QSslCertificate certificate);

        //! Action has failed.
        void onActionFailed(RError::Type errorType, QString errorMessage, QString message);

    signals:

        //! Task has finished and Cloud session has changed (client certificate was signed).
        void finished(RCloudSessionInfo sessionInfo);

        //! Task has failed.
        void failed(QString message);

};

#endif // RGL_OPEN_SSL_CSR_TASK_H
