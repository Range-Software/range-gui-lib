#ifndef RGL_CLOUD_SESSION_WIDGET_H
#define RGL_CLOUD_SESSION_WIDGET_H

#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include <rcl_cloud_client.h>
#include <rcl_cloud_session_info.h>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"
#include "rgl_file_chooser_button.h"

class RCloudSessionWidget : public QWidget
{

    Q_OBJECT

    protected:

        //! Cloud session info.
        RCloudSessionInfo sessionInfo;

        RCloudConnectionHandler *connectionHandler;
        RApplicationSettings *applicationSettings;

        RCloudClient *publicCloudClient;
        RCloudClient *privateCloudClient;


        //! Session name line edit.
        QLineEdit *nameEdit;

        //! Hostname line edit.
        QLineEdit *hostNameEdit;

        //! Public port spin box.
        QSpinBox *publicPortSpin;

        //! Private port spin box.
        QSpinBox *privatePortSpin;

        //! Timeout spin box.
        QSpinBox *timeoutSpin;

        //! Host certificate button.
        RFileChooserButton *hostCertificateFileButton;

        //! Private key file button.
        RFileChooserButton *clientPrivateKeyFileButton;

        //! Private key password line edit.
        QLineEdit *clientPrivateKeyPasswordEdit;

        //! Certificate file button.
        RFileChooserButton *clientCertificateFileButton;

        //! New button.
        QPushButton *newButton;

    public:

        //! Constructor.
        explicit RCloudSessionWidget(const RCloudSessionInfo &sessionInfo,
                                    RCloudConnectionHandler *connectionHandler,
                                    RApplicationSettings *applicationSettings,
                                    QWidget *parent = nullptr);

        //! Return const reference to cloud session info.
        const RCloudSessionInfo &getSessionInfo() const;

    public slots:

        //! Set session info.
        void setSessionInfo(const RCloudSessionInfo &sessionInfo);

    protected slots:

        //! Name changed.
        void onNameChanged(const QString &hostName);

        //! Hostname changed.
        void onHostNameChanged(const QString &hostName);

        //! Public port changed.
        void onPublicPortChanged(int publicPort);

        //! Private port changed.
        void onPrivatePortChanged(int privatePort);

        //! Timeout changed.
        void onTimeoutChanged(int timeout);

        //! CA Certificate file name changed.
        void onHostCertificateFileChanged(const QString &fileName);

        //! Private key file name changed.
        void onClientPrivateKeyFileChanged(const QString &fileName);

        //! Private key file name changed.
        void onClientPrivateKeyPasswordChanged(const QString &password);

        //! Certificate file name changed.
        void onClientCertificateFileChanged(const QString &fileName);

        //! Test public request button clicked.
        void onTestPublicRequestButtonClicked();

        //! Test private request button clicked.
        void onTestPrivateRequestButtonClicked();

        //! Public connection test finished.
        void onPublicConnectionTestFinished(QString message);

        //! Private connection test finished.
        void onPrivateConnectionTestFinished(QString message);

        //! Manage certificate clicked.
        void onClientCertificateRequestClicked();

        //! View private key clicked.
        void onClientPrivateKeyViewClicked();

        //! View host certificate clicked.
        void onHostCertificateViewClicked();

        //! View cliebnt certificate clicked.
        void onClientCertificateViewClicked();

        //! New identity (key, password and certificate) available.
        void onIdentityAvailable(const QString &keyPath, const QString &keyPassword, const QString &certificatePath);

    protected:

        //! View certificate.
        static void viewCertificates(const QString &certificatesPath);

    signals:

        void sessionInfoChanged(const RCloudSessionInfo &sessionInfo);

};

#endif // RGL_CLOUD_SESSION_WIDGET_H
