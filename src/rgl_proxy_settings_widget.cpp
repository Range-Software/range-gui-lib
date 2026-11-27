#include <QGridLayout>
#include <QLabel>

#include "rgl_proxy_settings_widget.h"

RProxySettingsWidget::RProxySettingsWidget(const RHttpProxySettings &proxySettings, QWidget *parent)
    : QWidget{parent}
    , proxySettings{proxySettings}
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setColumnStretch(1,1);
    this->setLayout(mainLayout);

    this->typeCombo = new QComboBox;
    this->typeCombo->addItem(tr("No proxy"),QVariant(RHttpProxySettings::NoProxy));
    this->typeCombo->addItem(tr("System"),QVariant(RHttpProxySettings::SystemProxy));
    this->typeCombo->addItem(tr("Manual"),QVariant(RHttpProxySettings::ManualProxy));
    this->typeCombo->setCurrentIndex(RProxySettingsWidget::proxyTypeToComboIndex(this->proxySettings.getType()));

    mainLayout->addWidget(new QLabel(tr("Type") + ":"),0,0);
    mainLayout->addWidget(this->typeCombo,0,1,1,3);

    this->hostEdit = new QLineEdit(this->proxySettings.getHost());
    this->hostEdit->setPlaceholderText("proxy.com");

    this->portSpin = new QSpinBox;
    this->portSpin->setRange(1,49151);
    this->portSpin->setValue(this->proxySettings.getPort());

    QLabel *hostLabel = new QLabel(tr("Host") + ":");
    QLabel *portLabel = new QLabel(tr("Port") + ":");

    mainLayout->addWidget(hostLabel,1,0);
    mainLayout->addWidget(this->hostEdit,1,1);
    mainLayout->addWidget(portLabel,1,2);
    mainLayout->addWidget(this->portSpin,1,3);

    this->userEdit = new QLineEdit(this->proxySettings.getUser());

    QLabel *userLabel = new QLabel(tr("User") + ":");

    mainLayout->addWidget(userLabel,2,0);
    mainLayout->addWidget(this->userEdit,2,1,1,3);

    this->passwordEdit = new QLineEdit(this->proxySettings.getPassword());
    this->passwordEdit->setEchoMode(QLineEdit::Password);

    QLabel *passwordLabel = new QLabel(tr("Password") + ":");

    mainLayout->addWidget(passwordLabel,3,0);
    mainLayout->addWidget(this->passwordEdit,3,1,1,3);

    this->manualProxyWidgets = {hostLabel,this->hostEdit,portLabel,this->portSpin,userLabel,this->userEdit,passwordLabel,this->passwordEdit};
    this->setManualProxyEnabled(this->proxySettings.getType() == RHttpProxySettings::ManualProxy);

    QObject::connect(this->typeCombo,&QComboBox::currentIndexChanged,this,&RProxySettingsWidget::onTypeComboIndexChanged);
    QObject::connect(this->hostEdit,&QLineEdit::textChanged,this,&RProxySettingsWidget::onHostChanged);
    QObject::connect(this->portSpin,&QSpinBox::valueChanged,this,&RProxySettingsWidget::onPortChanged);
    QObject::connect(this->userEdit,&QLineEdit::textChanged,this,&RProxySettingsWidget::onUserChanged);
    QObject::connect(this->passwordEdit,&QLineEdit::textChanged,this,&RProxySettingsWidget::onPasswordChanged);
}

int RProxySettingsWidget::proxyTypeToComboIndex(RHttpProxySettings::Type type)
{
    switch (type)
    {
        case RHttpProxySettings::NoProxy:
        {
            return 0;
        }
        case RHttpProxySettings::ManualProxy:
        {
            return 2;
        }
        case RHttpProxySettings::SystemProxy:
        default:
        {
            return 1;
        }
    }
}

void RProxySettingsWidget::setManualProxyEnabled(bool enabled)
{
    for (QWidget *widget : std::as_const(this->manualProxyWidgets))
    {
        widget->setEnabled(enabled);
    }
}

void RProxySettingsWidget::setDefaultValues()
{
    RHttpProxySettings defaultProxySettings;

    this->typeCombo->setCurrentIndex(RProxySettingsWidget::proxyTypeToComboIndex(defaultProxySettings.getType()));
    this->hostEdit->setText(defaultProxySettings.getHost());
    this->portSpin->setValue(defaultProxySettings.getPort());
    this->userEdit->setText(defaultProxySettings.getUser());
    this->passwordEdit->setText(defaultProxySettings.getPassword());
}

void RProxySettingsWidget::onTypeComboIndexChanged(int)
{
    RHttpProxySettings::Type type = RHttpProxySettings::Type(this->typeCombo->currentData().toInt());
    this->setManualProxyEnabled(type == RHttpProxySettings::ManualProxy);

    this->proxySettings.setType(type);
    emit this->proxyChanged(this->proxySettings);
}

void RProxySettingsWidget::onHostChanged(const QString &host)
{
    this->proxySettings.setHost(host);
    emit this->proxyChanged(this->proxySettings);
}

void RProxySettingsWidget::onPortChanged(int port)
{
    this->proxySettings.setPort(port);
    emit this->proxyChanged(this->proxySettings);
}

void RProxySettingsWidget::onUserChanged(const QString &user)
{
    this->proxySettings.setUser(user);
    emit this->proxyChanged(this->proxySettings);
}

void RProxySettingsWidget::onPasswordChanged(const QString &password)
{
    this->proxySettings.setPassword(password);
    emit this->proxyChanged(this->proxySettings);
}
