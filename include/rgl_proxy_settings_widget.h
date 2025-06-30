#ifndef RGL_PROXY_SETTINGS_WIDGET_H
#define RGL_PROXY_SETTINGS_WIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>

#include <rcl_http_proxy_settings.h>

class RProxySettingsWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Http proxy settings.
        RHttpProxySettings proxySettings;

        //! Type combo box.
        QComboBox *typeCombo;
        //! Form group box.
        QGroupBox *formGroupBox;
        //! Host line edit.
        QLineEdit *hostEdit;
        //! Port spin box.
        QSpinBox *portSpin;
        //! User line edit.
        QLineEdit *userEdit;
        //! Password line edit.
        QLineEdit *passwordEdit;

    public:

        //! Constructor.
        explicit RProxySettingsWidget(const RHttpProxySettings &proxySettings, QWidget *parent = nullptr);

    private:

        //! Convert proxy type to combo box index.
        static int proxyTypeToComboIndex(RHttpProxySettings::Type type);

    public slots:

        //! Set default values.
        void setDefaultValues();

    protected slots:

        //! Type combo box index changed.
        void onTypeComboIndexChanged(int);

        //! Host changed.
        void onHostChanged(const QString &host);

        //! Port changed.
        void onPortChanged(int port);

        //! User changed.
        void onUserChanged(const QString &user);

        //! Password changed.
        void onPasswordChanged(const QString &password);


    signals:

        void proxyChanged(const RHttpProxySettings &proxySettings);

};

#endif // RGL_PROXY_SETTINGS_WIDGET_H
