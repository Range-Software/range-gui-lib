#include <QFormLayout>
#include <QVBoxLayout>

#include "rgl_proxy_settings_widget.h"

RProxySettingsWidget::RProxySettingsWidget(const RHttpProxySettings &proxySettings, QWidget *parent)
    : QWidget{parent}
    , proxySettings{proxySettings}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->typeCombo = new QComboBox;
    this->typeCombo->addItem(tr("No proxy"),QVariant(RHttpProxySettings::NoProxy));
    this->typeCombo->addItem(tr("System"),QVariant(RHttpProxySettings::SystemProxy));
    this->typeCombo->addItem(tr("Manual"),QVariant(RHttpProxySettings::ManualProxy));
    this->typeCombo->setCurrentIndex(RProxySettingsWidget::proxyTypeToComboIndex(this->proxySettings.getType()));
    mainLayout->addWidget(this->typeCombo);

    this->formGroupBox = new QGroupBox;
    this->formGroupBox->setEnabled(this->proxySettings.getType() == RHttpProxySettings::ManualProxy);
    mainLayout->addWidget(this->formGroupBox);

    QFormLayout *formLayout = new QFormLayout;
    this->formGroupBox->setLayout(formLayout);

    this->hostEdit = new QLineEdit(this->proxySettings.getHost());
    this->hostEdit->setPlaceholderText("proxy.com");
    formLayout->addRow(tr("Host") + ":",this->hostEdit);

    this->portSpin = new QSpinBox;
    this->portSpin->setRange(1,49151);
    this->portSpin->setValue(this->proxySettings.getPort());
    formLayout->addRow(tr("Port") + ":",this->portSpin);

    this->userEdit = new QLineEdit(this->proxySettings.getUser());
    formLayout->addRow(tr("User") + ":",this->userEdit);

    this->passwordEdit = new QLineEdit(this->proxySettings.getPassword());
    this->passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow(tr("Password") + ":",this->passwordEdit);

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
    this->formGroupBox->setEnabled(type == RHttpProxySettings::ManualProxy);

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
