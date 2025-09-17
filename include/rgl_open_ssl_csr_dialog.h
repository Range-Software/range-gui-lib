#ifndef RGL_OPEN_SSL_CSR_DIALOG_H
#define RGL_OPEN_SSL_CSR_DIALOG_H

#include <QDialog>
#include <QObject>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <rcl_cloud_client.h>

#include "rgl_cloud_connection_handler.h"
#include "rgl_text_browser.h"

class ROpenSslCsrDialog : public QDialog
{

    Q_OBJECT

    protected:

        QString openSslToolPath;
        QString openSslCnfPath;
        QString keyPath;
        QString keyPassword;
        QString certificatePath;
        QString newKeyPath;
        QString newKeyPassword;
        QString newCsrPath;
        QString newCertificatePath;

        QComboBox *countryCombo;
        QLineEdit *stateEdit;
        QLineEdit *locationEdit;
        QLineEdit *organizationEdit;
        QLineEdit *organizationUnitEdit;
        QLineEdit *commonNameEdit;

        RTextBrowser *csrBrowser;
        QPushButton *directRequestButton;
        QPushButton *directRequestWithTokenButton;
        QPushButton *emailRequestButton;

        RCloudClient *publicCloudClient;
        RCloudClient *privateCloudClient;

    public:

        //! Constructor.
        explicit ROpenSslCsrDialog(RCloudConnectionHandler *connectionHandler,
                                   const QString &openSslToolPath,
                                   const QString &openSslCnfPath,
                                   const QString &keyPath,
                                   const QString &keyPassword,
                                   const QString &certificatePath,
                                   const QString &newKeyPath,
                                   const QString &newKeyPassword,
                                   const QString &newCsrPath,
                                   const QString &newCertificatePath,
                                   const QMap<QString,QString> &subjectMap,
                                   QWidget *parent = nullptr);

        //! Load private key from file.
        static QByteArray loadKey(const QString &openSslToolPath, const QString &fileName, const QString &password);

        //! Load csr key from file.
        static QByteArray loadCsr(const QString &fileName);

        //! Load certificate from file.
        static QByteArray loadCertificate(const QString &openSslToolPath, const QString &fileName);

    protected slots:

        //! New private key filename has changed.
        void onNewPrivateKeyFileNameChanged(const QString &fileName);

        //! New private key password has changed.
        void onNewPrivateKeyPasswordChanged(const QString &password);

        //! New certificate filename has changed.
        void onNewCertificateFileNameChanged(const QString &fileName);

        //! Generate CSR clicked.
        void onGenerateCsrClicked();

        //! CSR text changed.
        void onCsrTextChanged();

        //! Direct request clicked.
        void onDirectRequestClicked();

        //! Direct request with token clicked.
        void onDirectRequestWithTokenClicked();

        //! Email request clicked.
        void onEmailRequestClicked();

        //! Certificate was signed.
        void onSignedCertificate(QSslCertificate certificate);

    signals:

        //! New key, key password and cetificate are available.
        void identityAvailable(const QString &keyPath, const QString &keyPassword, const QString &certificatePath);

};

#endif // RGL_OPEN_SSL_CSR_DIALOG_H
