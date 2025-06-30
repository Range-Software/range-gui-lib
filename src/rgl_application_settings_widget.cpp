#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTabWidget>
#include <QStandardPaths>

#include <rbl_logger.h>

#include "rgl_application_settings_widget.h"
#include "rgl_application.h"
#include "rgl_message_box.h"

RApplicationSettingsWidget::RApplicationSettingsWidget(RApplicationSettings *applicationSettings, QWidget *parent)
    : QWidget(parent)
    , applicationSettings(applicationSettings)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->tabWidget = new QTabWidget;
    mainLayout->addWidget(this->tabWidget);

    // Appearance

    QWidget *apperanceWidget = new QWidget;
    this->tabWidget->addTab(apperanceWidget,tr("Appearance"));

    QFormLayout *appearanceLayout = new QFormLayout;
    apperanceWidget->setLayout(appearanceLayout);

    QStringList styles(RApplicationSettings::getStyles());
    this->styleCombo = new QComboBox;
    this->styleCombo->addItems(styles);
    for (int i=0;i<styles.size();i++)
    {
        if (styles.at(i) == this->applicationSettings->getStyle())
        {
            this->styleCombo->setCurrentIndex(i);
        }
    }
    appearanceLayout->addRow(tr("Style") + ":",this->styleCombo);

    QStringList languageNames(RApplicationSettings::getLanguageNames());
    this->languageCombo = new QComboBox;
    this->languageCombo->addItems(languageNames);
    for (int i=0;i<languageNames.size();i++)
    {
        if (RApplicationSettings::languageNameToCode(languageNames.at(i)) == this->applicationSettings->getLanguageCode())
        {
            this->languageCombo->setCurrentIndex(i);
        }
    }
    appearanceLayout->addRow(tr("Language") + ":",this->languageCombo);

    this->formatCombo = new QComboBox;
    for (int i=0;i<RApplicationSettings::Format::nFormats;i++)
    {
        this->formatCombo->addItem(RApplicationSettings::formatToName(RApplicationSettings::Format(i)));
    }
    this->formatCombo->setCurrentIndex(this->applicationSettings->getFormat());
    appearanceLayout->addRow(tr("Formats") + ":",this->formatCombo);

    appearanceLayout->addRow(new QWidget);

#ifdef _R_PERSONAL_INFO_ENABLED
    // Personal

    QWidget *personalWidget = new QWidget;
    this->tabWidget->addTab(personalWidget,tr("Personal"));

    QFormLayout *personalLayout = new QFormLayout;
    personalWidget->setLayout(personalLayout);

    this->personalNameEdit = new QLineEdit(this->applicationSettings->getPersonalName());
    this->personalNameEdit->setPlaceholderText(tr("Jon Smith"));

    personalLayout->addRow(tr("Full name") + ":",this->personalNameEdit);

    this->personalEmailEdit = new QLineEdit(this->applicationSettings->getPersonalEmail());
    this->personalEmailEdit->setPlaceholderText("example@domain.com");

    QRegularExpression rex("^[a-zA-Z0-9](?:[a-zA-Z0-9._%+-]*[a-zA-Z0-9])?@[a-zA-Z0-9](?:[a-zA-Z0-9.-]*[a-zA-Z0-9])?\\.[a-zA-Z]{2,}$");
    QValidator *emailValidator = new QRegularExpressionValidator(rex, this);
    this->personalEmailEdit->setValidator(emailValidator);

    personalLayout->addRow(tr("Personal e-mail") + ":",this->personalEmailEdit);

    personalLayout->addRow(new QWidget);
#endif

    // Network

    QWidget *networkWidget = new QWidget;
    this->tabWidget->addTab(networkWidget,tr("Network"));

    QVBoxLayout *networkLayout = new QVBoxLayout;
    networkWidget->setLayout(networkLayout);

    QGroupBox *proxyGroupBox = new QGroupBox(tr("Network proxy"));
    networkLayout->addWidget(proxyGroupBox);

    QVBoxLayout *proxyLayout = new QVBoxLayout;
    proxyGroupBox->setLayout(proxyLayout);

    this->proxySettingsWidget = new RProxySettingsWidget(this->applicationSettings->getProxySettings());
    proxyLayout->addWidget(this->proxySettingsWidget);

    QGroupBox *cloudGroupBox = new QGroupBox(tr("Cloud"));
    networkLayout->addWidget(cloudGroupBox);

    QGridLayout *cloudLayout = new QGridLayout;
    cloudGroupBox->setLayout(cloudLayout);

    this->cloudRefreshTimeoutSpin = new QSpinBox;
    this->cloudRefreshTimeoutSpin->setRange(0,INT_MAX);
    this->cloudRefreshTimeoutSpin->setValue(this->applicationSettings->getCloudRefreshTimeout()/1000);

    cloudLayout->addWidget(new QLabel(tr("Refresh timeout") + ":"),0,0,1,1);
    cloudLayout->addWidget(this->cloudRefreshTimeoutSpin,0,1,1,1);
    cloudLayout->addWidget(new QLabel("[seconds]"),0,2,1,1);

#ifdef _R_CLOUD_SEND_USAGE_ENABLED
    this->cloudSendUsageInfoCheckBox = new QCheckBox(tr("Send usage information"));
    this->cloudSendUsageInfoCheckBox->setChecked(this->applicationSettings->getCloudSendUsageInfo());

    cloudLayout->addWidget(this->cloudSendUsageInfoCheckBox,1,0,1,3);
#endif

    networkLayout->addStretch(10);

    // Paths

    QWidget *pathsWidget = new QWidget;
    this->tabWidget->addTab(pathsWidget,tr("Paths"));

    QVBoxLayout *pathsLayout = new QVBoxLayout;
    pathsWidget->setLayout(pathsLayout);

    this->helpDirectoryButton = new RFileChooserButton(tr("Help directory") + ":",RFileChooserButton::Directory);
    this->helpDirectoryButton->setFileName(this->applicationSettings->getHelpDir());
    this->helpDirectoryButton->setSearchDirectory(this->applicationSettings->findHelpDir());
    pathsLayout->addWidget(this->helpDirectoryButton);

    this->opensslToolFileButton = new RFileChooserButton(tr("OpenSSL tool") + ":",RFileChooserButton::OpenFile);
    this->opensslToolFileButton->setFileName(this->applicationSettings->getOpensslToolPath());
    this->opensslToolFileButton->setSearchDirectory(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
    pathsLayout->addWidget(this->opensslToolFileButton);

    pathsLayout->addStretch(10);

    // Keyboard shortcuts

    QWidget *keyboardShortcusWidget = new QWidget;
    this->tabWidget->addTab(keyboardShortcusWidget,tr("Keyboard shortcuts"));

    QVBoxLayout *keyboardShortcutsLayout = new QVBoxLayout;
    keyboardShortcusWidget->setLayout(keyboardShortcutsLayout);

    this->keyboardShortcutsEdirotWidget = new RKeyboardShortcutsEditorWidget(RApplication::instance()->getActionList());
    keyboardShortcutsLayout->addWidget(this->keyboardShortcutsEdirotWidget);

    QObject::connect(this->styleCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onStyleChanged);
    QObject::connect(this->languageCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onLanguageChanged);
    QObject::connect(this->formatCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onFormatChanged);
    QObject::connect(this->proxySettingsWidget,&RProxySettingsWidget::proxyChanged,this,&RApplicationSettingsWidget::onProxyChanged);
    QObject::connect(this->cloudRefreshTimeoutSpin,&QSpinBox::valueChanged,this,&RApplicationSettingsWidget::onCloudRefreshTimeoutChanged);
#ifdef _R_CLOUD_SEND_USAGE_ENABLED
    QObject::connect(this->cloudSendUsageInfoCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onCloudSendUsageInfoChanged);
#endif
#ifdef _R_PERSONAL_INFO_ENABLED
    QObject::connect(this->personalNameEdit,&QLineEdit::textChanged,this,&RApplicationSettingsWidget::onPersonalNameChanged);
    QObject::connect(this->personalEmailEdit,&QLineEdit::textChanged,this,&RApplicationSettingsWidget::onPersonalEmailChanged);
#endif
    QObject::connect(this->helpDirectoryButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onHelpDirectoryChanged);
    QObject::connect(this->opensslToolFileButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onOpensslToolFileChanged);
    QObject::connect(this->keyboardShortcutsEdirotWidget,&RKeyboardShortcutsEditorWidget::shortcutChanged,this,&RApplicationSettingsWidget::onKeyboardShortcutChanged);
}

void RApplicationSettingsWidget::setDefaultValues()
{
    for (int i=0;i<this->styleCombo->count();i++)
    {
        if (this->styleCombo->itemText(i) == RApplicationSettings::getDefaultStyle())
        {
            this->styleCombo->setCurrentIndex(i);
        }
    }

    for (int i=0;i<this->languageCombo->count();i++)
    {
        if (RApplicationSettings::languageNameToCode(this->languageCombo->itemText(i)) == RApplicationSettings::getDefaultLanguageCode())
        {
            this->languageCombo->setCurrentIndex(i);
        }
    }

    this->proxySettingsWidget->setDefaultValues();
    this->cloudRefreshTimeoutSpin->setValue(RApplicationSettings::getDefaultCloudRefreshTimeout()/1000);
#ifdef _R_CLOUD_SEND_USAGE_ENABLED
    this->cloudSendUsageInfoCheckBox->setCheckState(RApplicationSettings::getDefaultCloudSendUsageInfo() ? Qt::Checked : Qt::Unchecked);
#endif
#ifdef _R_PERSONAL_INFO_ENABLED
    this->personalNameEdit->setText(QString());
    this->personalEmailEdit->setText(QString());
#endif
    this->helpDirectoryButton->setFileName(this->applicationSettings->findHelpDir());
    this->opensslToolFileButton->setFileName(this->applicationSettings->getDefaultOpensslToolPath());
    this->keyboardShortcutsEdirotWidget->resetToDefault();
}

void RApplicationSettingsWidget::onStyleChanged(const QString &style)
{
    this->applicationSettings->setStyle(style);
}

void RApplicationSettingsWidget::onLanguageChanged(const QString &languageName)
{
    this->applicationSettings->setLanguageCode(RApplicationSettings::languageNameToCode(languageName));
    RMessageBox::information(this,tr("Language change"),tr("Language change requires an application restart."));
}

void RApplicationSettingsWidget::onFormatChanged(const QString &formatName)
{
    this->applicationSettings->setFormat(RApplicationSettings::Format(this->formatCombo->currentIndex()));
    RMessageBox::information(this,tr("Format change"),tr("Format change requires an application restart."));
}

void RApplicationSettingsWidget::onProxyChanged(const RHttpProxySettings &proxySettings)
{
    this->applicationSettings->setProxySettings(proxySettings);
}

void RApplicationSettingsWidget::onCloudRefreshTimeoutChanged(int cloudRefreshTimeout)
{
    this->applicationSettings->setCloudRefreshTimeout(cloudRefreshTimeout*1000);
}

#ifdef _R_CLOUD_SEND_USAGE_ENABLED
void RApplicationSettingsWidget::onCloudSendUsageInfoChanged(Qt::CheckState state)
{
    this->applicationSettings->setCloudSendUsageInfo(state == Qt::Checked);
}
#endif

#ifdef _R_PERSONAL_INFO_ENABLED
void RApplicationSettingsWidget::onPersonalNameChanged(const QString &personalName)
{
    this->applicationSettings->setPersonalName(personalName);
}

void RApplicationSettingsWidget::onPersonalEmailChanged(const QString &personalEmail)
{
    this->applicationSettings->setPersonalEmail(personalEmail);
}
#endif

void RApplicationSettingsWidget::onHelpDirectoryChanged(const QString &directory)
{
    this->applicationSettings->setHelpDir(directory);
}

void RApplicationSettingsWidget::onOpensslToolFileChanged(const QString &fileName)
{
    this->applicationSettings->setOpensslToolPath(fileName);
}

void RApplicationSettingsWidget::onKeyboardShortcutChanged(uint position, const QString &shortcut)
{
    RApplication::instance()->getActionList()->changeShortcut(position,shortcut);
}
