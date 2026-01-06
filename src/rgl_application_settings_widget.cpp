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
#include "rgl_style.h"

RApplicationSettingsWidget::RApplicationSettingsWidget(RApplicationSettings *applicationSettings, QWidget *parent)
    : QWidget{parent}
    , applicationSettings{applicationSettings}
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

    QList<Qt::ColorScheme> colorSchemes(RApplicationSettings::getColorSchemes());
    this->colorSchemeCombo = new QComboBox;
    for (int i=0;i<colorSchemes.size();i++)
    {
        this->colorSchemeCombo->addItem(RStyle::colorSchemeToText(colorSchemes.at(i)),int(colorSchemes.at(i)));
    }
    this->colorSchemeCombo->setCurrentIndex(int(this->applicationSettings->getColorScheme()));
    appearanceLayout->addRow(tr("Color scheme") + ":",this->colorSchemeCombo);

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
        this->formatCombo->addItem(RApplicationSettings::formatToName(RApplicationSettings::Format(i)),RApplicationSettings::formatToCode(RApplicationSettings::Format(i)));
    }
    this->formatCombo->setCurrentIndex(this->applicationSettings->getFormat());
    appearanceLayout->addRow(tr("Formats") + ":",this->formatCombo);

    appearanceLayout->addRow(new QWidget);

    // User

    QWidget *userWidget = new QWidget;
    this->tabWidget->addTab(userWidget,tr("User"));

    QFormLayout *userLayout = new QFormLayout;
    userWidget->setLayout(userLayout);

    this->userFullNameEdit = new QLineEdit(this->applicationSettings->getUserFullName());
    this->userFullNameEdit->setPlaceholderText(tr("Jon Smith"));

    userLayout->addRow(tr("Full name") + ":",this->userFullNameEdit);

    this->userEmailEdit = new QLineEdit(this->applicationSettings->getUserEmail());
    this->userEmailEdit->setPlaceholderText("example@domain.com");

    QRegularExpression rex("^[a-zA-Z0-9](?:[a-zA-Z0-9._%+-]*[a-zA-Z0-9])?@[a-zA-Z0-9](?:[a-zA-Z0-9.-]*[a-zA-Z0-9])?\\.[a-zA-Z]{2,}$");
    QValidator *emailValidator = new QRegularExpressionValidator(rex, this);
    this->userEmailEdit->setValidator(emailValidator);

    userLayout->addRow(tr("E-mail") + ":",this->userEmailEdit);

    this->userTerritoryCombo = new RTerritoryComboBox(this->applicationSettings->getUserTerritory());
    userLayout->addRow(tr("Country") + ":",this->userTerritoryCombo);

    userLayout->addRow(new QWidget);

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

    this->cloudSyncDataDirectoryCheckBox = new QCheckBox(tr("Synchronize data directory"));
    this->cloudSyncDataDirectoryCheckBox->setChecked(this->applicationSettings->getCloudSyncDataDirectory());

    cloudLayout->addWidget(this->cloudSyncDataDirectoryCheckBox,1,0,1,3);

    QGroupBox *softwareGroupBox = new QGroupBox(tr("Software"));
    networkLayout->addWidget(softwareGroupBox);

    QVBoxLayout *softwareLayout = new QVBoxLayout;
    softwareGroupBox->setLayout(softwareLayout);

    this->softwareSendUsageInfoCheckBox = new QCheckBox(tr("Send usage information"));
    this->softwareSendUsageInfoCheckBox->setChecked(this->applicationSettings->getSoftwareSendUsageInfo());

    softwareLayout->addWidget(this->softwareSendUsageInfoCheckBox);

    this->softwareCheckUpdatesCheckBox = new QCheckBox(tr("Check for software update"));
    this->softwareCheckUpdatesCheckBox->setChecked(this->applicationSettings->getSoftwareCheckUpdates());

    softwareLayout->addWidget(this->softwareCheckUpdatesCheckBox);

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

    this->opensslConfFileButton = new RFileChooserButton(tr("OpenSSL configuration") + ":",RFileChooserButton::OpenFile);
    this->opensslConfFileButton->setFileName(this->applicationSettings->getOpensslConfPath());
    this->opensslConfFileButton->setSearchDirectory(this->applicationSettings->findEtcDir());
    pathsLayout->addWidget(this->opensslConfFileButton);

    pathsLayout->addStretch(10);

    // Keyboard shortcuts

    QWidget *keyboardShortcusWidget = new QWidget;
    this->tabWidget->addTab(keyboardShortcusWidget,tr("Keyboard shortcuts"));

    QVBoxLayout *keyboardShortcutsLayout = new QVBoxLayout;
    keyboardShortcusWidget->setLayout(keyboardShortcutsLayout);

    this->keyboardShortcutsEdirotWidget = new RKeyboardShortcutsEditorWidget(RApplication::instance()->getActionList());
    keyboardShortcutsLayout->addWidget(this->keyboardShortcutsEdirotWidget);

    QObject::connect(this->styleCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onStyleChanged);
    QObject::connect(this->colorSchemeCombo,&QComboBox::currentIndexChanged,this,&RApplicationSettingsWidget::onColorSchemeChanged);
    QObject::connect(this->languageCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onLanguageChanged);
    QObject::connect(this->formatCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onFormatChanged);
    QObject::connect(this->proxySettingsWidget,&RProxySettingsWidget::proxyChanged,this,&RApplicationSettingsWidget::onProxyChanged);
    QObject::connect(this->cloudRefreshTimeoutSpin,&QSpinBox::valueChanged,this,&RApplicationSettingsWidget::onCloudRefreshTimeoutChanged);
    QObject::connect(this->cloudSyncDataDirectoryCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onCloudSyncDataDirectoryChanged);
    QObject::connect(this->softwareSendUsageInfoCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onSoftwareSendUsageInfoChanged);
    QObject::connect(this->softwareCheckUpdatesCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onSoftwareCheckUpdatesChanged);
    QObject::connect(this->userFullNameEdit,&QLineEdit::textChanged,this,&RApplicationSettingsWidget::onUserFullNameChanged);
    QObject::connect(this->userEmailEdit,&QLineEdit::textChanged,this,&RApplicationSettingsWidget::onUserEmailChanged);
    QObject::connect(this->userTerritoryCombo,&RTerritoryComboBox::territorrySelected,this,&RApplicationSettingsWidget::onUserTerritorySelected);
    QObject::connect(this->helpDirectoryButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onHelpDirectoryChanged);
    QObject::connect(this->opensslToolFileButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onOpensslToolFileChanged);
    QObject::connect(this->opensslConfFileButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onOpensslConfFileChanged);
    QObject::connect(this->keyboardShortcutsEdirotWidget,&RKeyboardShortcutsEditorWidget::shortcutChanged,this,&RApplicationSettingsWidget::onKeyboardShortcutChanged);
}

void RApplicationSettingsWidget::setDefaultValues()
{
    for (int i=0;i<this->styleCombo->count();i++)
    {
        if (this->styleCombo->itemText(i) == RApplicationSettings::getDefaultStyle())
        {
            this->styleCombo->setCurrentIndex(i);
            break;
        }
    }

    for (int i=0;i<this->languageCombo->count();i++)
    {
        if (RApplicationSettings::languageNameToCode(this->languageCombo->itemText(i)) == RApplicationSettings::getDefaultLanguageCode())
        {
            this->languageCombo->setCurrentIndex(i);
            break;
        }
    }

    for (int i=0;i<this->formatCombo->count();i++)
    {
        if (this->formatCombo->itemData(i).toString() == RApplicationSettings::formatToCode(RApplicationSettings::Format(i)))
        {
            this->formatCombo->setCurrentIndex(i);
            break;
        }
    }

    this->proxySettingsWidget->setDefaultValues();
    this->cloudRefreshTimeoutSpin->setValue(RApplicationSettings::getDefaultCloudRefreshTimeout()/1000);
    this->cloudSyncDataDirectoryCheckBox->setCheckState(RApplicationSettings::getDefaultCloudSyncDataDirectory() ? Qt::Checked : Qt::Unchecked);
    this->softwareSendUsageInfoCheckBox->setCheckState(RApplicationSettings::getDefaultSoftwareSendUsageInfo() ? Qt::Checked : Qt::Unchecked);
    this->softwareCheckUpdatesCheckBox->setCheckState(RApplicationSettings::getDefaultSoftwareCheckUpdates() ? Qt::Checked : Qt::Unchecked);
    this->userFullNameEdit->setText(QString());
    this->userEmailEdit->setText(QString());
    this->userTerritoryCombo->setCurrentTerritoryCode(RApplicationSettings::getDefaultUserTerritory());
    this->helpDirectoryButton->setFileName(this->applicationSettings->findHelpDir());
    this->opensslToolFileButton->setFileName(this->applicationSettings->getDefaultOpensslToolPath());
    this->keyboardShortcutsEdirotWidget->resetToDefault();
}

void RApplicationSettingsWidget::onStyleChanged(const QString &style)
{
    this->applicationSettings->setStyle(style);
}

void RApplicationSettingsWidget::onColorSchemeChanged(int currentIndex)
{
    this->applicationSettings->setColorScheme(Qt::ColorScheme(currentIndex));
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

void RApplicationSettingsWidget::onCloudSyncDataDirectoryChanged(Qt::CheckState state)
{
    this->applicationSettings->setCloudSyncDataDirectory(state == Qt::Checked);
}

void RApplicationSettingsWidget::onSoftwareSendUsageInfoChanged(Qt::CheckState state)
{
    this->applicationSettings->setSoftwareSendUsageInfo(state == Qt::Checked);
}

void RApplicationSettingsWidget::onSoftwareCheckUpdatesChanged(Qt::CheckState state)
{
    this->applicationSettings->setSoftwareCheckUpdates(state == Qt::Checked);
}

void RApplicationSettingsWidget::onUserFullNameChanged(const QString &userFullName)
{
    this->applicationSettings->setUserFullName(userFullName);
}

void RApplicationSettingsWidget::onUserEmailChanged(const QString &userEmail)
{
    this->applicationSettings->setUserEmail(userEmail);
}

void RApplicationSettingsWidget::onUserTerritorySelected(const QString &userTerritory)
{
    this->applicationSettings->setUserTerritory(userTerritory);
}

void RApplicationSettingsWidget::onHelpDirectoryChanged(const QString &directory)
{
    this->applicationSettings->setHelpDir(directory);
}

void RApplicationSettingsWidget::onOpensslToolFileChanged(const QString &fileName)
{
    this->applicationSettings->setOpensslToolPath(fileName);
}

void RApplicationSettingsWidget::onOpensslConfFileChanged(const QString &fileName)
{
    this->applicationSettings->setOpensslConfPath(fileName);
}

void RApplicationSettingsWidget::onKeyboardShortcutChanged(uint position, const QString &shortcut)
{
    RApplication::instance()->getActionList()->changeShortcut(position,shortcut);
}
