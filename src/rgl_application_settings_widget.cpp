#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QStackedWidget>
#include <QStyle>
#include <QStandardPaths>

#include <rbl_logger.h>
#include <rcl_open_ssl_tool.h>

#include "rgl_application_settings_widget.h"
#include "rgl_application.h"
#include "rgl_message_box.h"
#include "rgl_style.h"

RApplicationSettingsWidget::RApplicationSettingsWidget(RApplicationSettings *applicationSettings, QWidget *parent)
    : QWidget{parent}
    , applicationSettings{applicationSettings}
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    this->pageList = new QListWidget;
    mainLayout->addWidget(this->pageList);

    this->pageStack = new QStackedWidget;
    mainLayout->addWidget(this->pageStack,1);

    // General

    QWidget *generalWidget = new QWidget;
    this->addPage(generalWidget,tr("General"));

    QFormLayout *generalLayout = new QFormLayout;
    generalLayout->setContentsMargins(0,0,0,0);
    generalWidget->setLayout(generalLayout);

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
    generalLayout->addRow(tr("Style") + ":",this->styleCombo);

    QList<Qt::ColorScheme> colorSchemes(RApplicationSettings::getColorSchemes());
    this->colorSchemeCombo = new QComboBox;
    for (int i=0;i<colorSchemes.size();i++)
    {
        this->colorSchemeCombo->addItem(RStyle::colorSchemeToText(colorSchemes.at(i)),int(colorSchemes.at(i)));
    }
    this->colorSchemeCombo->setCurrentIndex(int(this->applicationSettings->getColorScheme()));
    generalLayout->addRow(tr("Color scheme") + ":",this->colorSchemeCombo);

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
    generalLayout->addRow(tr("Language") + ":",this->languageCombo);

    this->formatCombo = new QComboBox;
    for (int i=0;i<RApplicationSettings::Format::nFormats;i++)
    {
        this->formatCombo->addItem(RApplicationSettings::formatToName(RApplicationSettings::Format(i)),RApplicationSettings::formatToCode(RApplicationSettings::Format(i)));
    }
    this->formatCombo->setCurrentIndex(this->applicationSettings->getFormat());
    generalLayout->addRow(tr("Formats") + ":",this->formatCombo);

    this->dontUseNativeMenuBarCheckBox = nullptr;
    if (RApplicationSettings::isNativeMenuBarSupported())
    {
        this->dontUseNativeMenuBarCheckBox = new QCheckBox(tr("Do not use native menubar"));
        this->dontUseNativeMenuBarCheckBox->setToolTip(tr("Show the menubar inside the application window instead of the system menubar."));
        this->dontUseNativeMenuBarCheckBox->setChecked(this->applicationSettings->getDontUseNativeMenuBar());
        generalLayout->addRow(tr("Menubar") + ":",this->dontUseNativeMenuBarCheckBox);
    }

    // User

    QWidget *userWidget = new QWidget;
    this->addPage(userWidget,tr("User"));

    QFormLayout *userLayout = new QFormLayout;
    userLayout->setContentsMargins(0,0,0,0);
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

    // Network

    QWidget *networkWidget = new QWidget;
    this->addPage(networkWidget,tr("Network"));

    QVBoxLayout *networkLayout = new QVBoxLayout;
    networkLayout->setContentsMargins(0,0,0,0);
    networkWidget->setLayout(networkLayout);

    QGroupBox *proxyGroupBox = new QGroupBox(tr("Network proxy"));
    networkLayout->addWidget(proxyGroupBox);

    QVBoxLayout *proxyLayout = new QVBoxLayout;
    proxyGroupBox->setLayout(proxyLayout);

    this->proxySettingsWidget = new RProxySettingsWidget(this->applicationSettings->getProxySettings());
    proxyLayout->addWidget(this->proxySettingsWidget);

    QGroupBox *cloudGroupBox = new QGroupBox(tr("Cloud"));
    networkLayout->addWidget(cloudGroupBox);

    QFormLayout *cloudLayout = new QFormLayout;
    cloudGroupBox->setLayout(cloudLayout);

    this->cloudRefreshTimeoutSpin = new QSpinBox;
    this->cloudRefreshTimeoutSpin->setRange(0,INT_MAX);
    this->cloudRefreshTimeoutSpin->setSuffix(" " + tr("seconds"));
    this->cloudRefreshTimeoutSpin->setValue(this->applicationSettings->getCloudRefreshTimeout()/1000);

    cloudLayout->addRow(tr("Refresh timeout") + ":",this->cloudRefreshTimeoutSpin);

    this->cloudSyncDataDirectoryCheckBox = new QCheckBox(tr("Synchronize data directory"));
    this->cloudSyncDataDirectoryCheckBox->setChecked(this->applicationSettings->getCloudSyncDataDirectory());

    cloudLayout->addRow(this->cloudSyncDataDirectoryCheckBox);

    this->cloudClientCertificateRenewCheckBox = new QCheckBox(tr("Renew client certificate"));
    this->cloudClientCertificateRenewCheckBox->setChecked(this->applicationSettings->getCloudClientCertificateRenew());

    cloudLayout->addRow(this->cloudClientCertificateRenewCheckBox);

    this->cloudClientCertificateExpiryDaysSpin = new QSpinBox;
    this->cloudClientCertificateExpiryDaysSpin->setRange(0,INT_MAX);
    this->cloudClientCertificateExpiryDaysSpin->setSuffix(" " + tr("days"));
    this->cloudClientCertificateExpiryDaysSpin->setValue(this->applicationSettings->getCloudClientCertificateExpiryDays());

    // Indented so it reads as a sub-option of the renew check box above it.
    this->cloudClientCertificateExpiryDaysLabel = new QLabel(tr("Days before expiry") + ":");
    this->cloudClientCertificateExpiryDaysLabel->setIndent(this->style()->pixelMetric(QStyle::PM_IndicatorWidth) + this->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing));

    cloudLayout->addRow(this->cloudClientCertificateExpiryDaysLabel,this->cloudClientCertificateExpiryDaysSpin);

    this->cloudClientCertificateExpiryDaysLabel->setEnabled(this->cloudClientCertificateRenewCheckBox->isChecked());
    this->cloudClientCertificateExpiryDaysSpin->setEnabled(this->cloudClientCertificateRenewCheckBox->isChecked());

    networkLayout->addStretch(10);

    // Privacy

    QWidget *privacyWidget = new QWidget;
    this->addPage(privacyWidget,tr("Privacy"));

    QVBoxLayout *privacyLayout = new QVBoxLayout;
    privacyLayout->setContentsMargins(0,0,0,0);
    privacyWidget->setLayout(privacyLayout);

    QGroupBox *softwareGroupBox = new QGroupBox(tr("Software"));
    privacyLayout->addWidget(softwareGroupBox);

    QVBoxLayout *softwareLayout = new QVBoxLayout;
    softwareGroupBox->setLayout(softwareLayout);

    this->softwareSendUsageInfoCheckBox = new QCheckBox(tr("Send usage information"));
    this->softwareSendUsageInfoCheckBox->setChecked(this->applicationSettings->getSoftwareSendUsageInfo());

    softwareLayout->addWidget(this->softwareSendUsageInfoCheckBox);

    this->softwareCheckUpdatesCheckBox = new QCheckBox(tr("Check for software update"));
    this->softwareCheckUpdatesCheckBox->setChecked(this->applicationSettings->getSoftwareCheckUpdates());

//ifdef STORE_BUILD
    // A store build carries no update checker, so the option would do nothing.
    // The check box is kept alive and merely hidden, so the slots connected to
    // it below and setDefaultValues() need no special case.
    this->softwareCheckUpdatesCheckBox->setVisible(false);
//endif
    softwareLayout->addWidget(this->softwareCheckUpdatesCheckBox);

    QGroupBox *aiGroupBox = new QGroupBox(tr("AI services"));
    privacyLayout->addWidget(aiGroupBox);

    QVBoxLayout *aiLayout = new QVBoxLayout;
    aiGroupBox->setLayout(aiLayout);

    QLabel *aiConsentLabel = new QLabel(tr("Before data is sent to an AI service for the first time, you are asked for permission. Withdraw all given permissions to be asked again."));
    aiConsentLabel->setWordWrap(true);
    aiLayout->addWidget(aiConsentLabel);

    this->aiConsentClearButton = new QPushButton(tr("Withdraw AI data sharing permissions"));
    this->aiConsentClearButton->setEnabled(this->applicationSettings->hasAiDataSharingConsent());
    aiLayout->addWidget(this->aiConsentClearButton,0,Qt::AlignLeft);

    privacyLayout->addStretch(10);

    // Paths

    QWidget *pathsWidget = new QWidget;
    this->addPage(pathsWidget,tr("Paths"));

    QVBoxLayout *pathsLayout = new QVBoxLayout;
    pathsLayout->setContentsMargins(0,0,0,0);
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

    // Keys and CSRs are generated without the openssl tool
    this->opensslToolFileButton->setVisible(ROpenSslTool::isToolRequired());
    this->opensslConfFileButton->setVisible(ROpenSslTool::isToolRequired());

    pathsLayout->addStretch(10);

    // Keyboard shortcuts

    QWidget *keyboardShortcusWidget = new QWidget;
    this->addPage(keyboardShortcusWidget,tr("Keyboard shortcuts"));

    QVBoxLayout *keyboardShortcutsLayout = new QVBoxLayout;
    keyboardShortcutsLayout->setContentsMargins(0,0,0,0);
    keyboardShortcusWidget->setLayout(keyboardShortcutsLayout);

    this->keyboardShortcutsEdirotWidget = new RKeyboardShortcutsEditorWidget(RApplication::instance()->getActionList());
    keyboardShortcutsLayout->addWidget(this->keyboardShortcutsEdirotWidget);

    // Wide enough for the longest page name, so none is cut off.
    this->pageList->setFixedWidth(this->pageList->sizeHintForColumn(0) + 2 * this->pageList->frameWidth() + 24);
    this->pageList->setCurrentRow(0);

    QObject::connect(this->pageList,&QListWidget::currentRowChanged,this->pageStack,&QStackedWidget::setCurrentIndex);
    QObject::connect(this->styleCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onStyleChanged);
    QObject::connect(this->colorSchemeCombo,&QComboBox::currentIndexChanged,this,&RApplicationSettingsWidget::onColorSchemeChanged);
    QObject::connect(this->languageCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onLanguageChanged);
    QObject::connect(this->formatCombo,&QComboBox::currentTextChanged,this,&RApplicationSettingsWidget::onFormatChanged);
    if (this->dontUseNativeMenuBarCheckBox)
    {
        QObject::connect(this->dontUseNativeMenuBarCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onDontUseNativeMenuBarChanged);
    }
    QObject::connect(this->proxySettingsWidget,&RProxySettingsWidget::proxyChanged,this,&RApplicationSettingsWidget::onProxyChanged);
    QObject::connect(this->cloudRefreshTimeoutSpin,&QSpinBox::valueChanged,this,&RApplicationSettingsWidget::onCloudRefreshTimeoutChanged);
    QObject::connect(this->cloudSyncDataDirectoryCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onCloudSyncDataDirectoryChanged);
    QObject::connect(this->cloudClientCertificateRenewCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onCloudClientCertificateRenewChanged);
    QObject::connect(this->cloudClientCertificateExpiryDaysSpin,&QSpinBox::valueChanged,this,&RApplicationSettingsWidget::onCloudClientCertificateExpiryDaysChanged);
    QObject::connect(this->softwareSendUsageInfoCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onSoftwareSendUsageInfoChanged);
    QObject::connect(this->softwareCheckUpdatesCheckBox,&QCheckBox::checkStateChanged,this,&RApplicationSettingsWidget::onSoftwareCheckUpdatesChanged);
    QObject::connect(this->aiConsentClearButton,&QPushButton::clicked,this,&RApplicationSettingsWidget::onAiConsentClearClicked);
    QObject::connect(this->userFullNameEdit,&QLineEdit::textChanged,this,&RApplicationSettingsWidget::onUserFullNameChanged);
    QObject::connect(this->userEmailEdit,&QLineEdit::textChanged,this,&RApplicationSettingsWidget::onUserEmailChanged);
    QObject::connect(this->userTerritoryCombo,&RTerritoryComboBox::territorrySelected,this,&RApplicationSettingsWidget::onUserTerritorySelected);
    QObject::connect(this->helpDirectoryButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onHelpDirectoryChanged);
    QObject::connect(this->opensslToolFileButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onOpensslToolFileChanged);
    QObject::connect(this->opensslConfFileButton,&RFileChooserButton::fileNameChanged,this,&RApplicationSettingsWidget::onOpensslConfFileChanged);
    QObject::connect(this->keyboardShortcutsEdirotWidget,&RKeyboardShortcutsEditorWidget::shortcutChanged,this,&RApplicationSettingsWidget::onKeyboardShortcutChanged);
}

void RApplicationSettingsWidget::addPage(QWidget *pageWidget, const QString &title)
{
    this->pageList->addItem(title);
    this->pageStack->addWidget(pageWidget);
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

    if (this->dontUseNativeMenuBarCheckBox)
    {
        this->dontUseNativeMenuBarCheckBox->setCheckState(RApplicationSettings::getDefaultDontUseNativeMenuBar() ? Qt::Checked : Qt::Unchecked);
    }

    this->proxySettingsWidget->setDefaultValues();
    this->cloudRefreshTimeoutSpin->setValue(RApplicationSettings::getDefaultCloudRefreshTimeout()/1000);
    this->cloudSyncDataDirectoryCheckBox->setCheckState(RApplicationSettings::getDefaultCloudSyncDataDirectory() ? Qt::Checked : Qt::Unchecked);
    this->cloudClientCertificateRenewCheckBox->setCheckState(RApplicationSettings::getDefaultCloudClientCertificateRenew() ? Qt::Checked : Qt::Unchecked);
    this->softwareSendUsageInfoCheckBox->setCheckState(RApplicationSettings::getDefaultSoftwareSendUsageInfo() ? Qt::Checked : Qt::Unchecked);
    this->softwareCheckUpdatesCheckBox->setCheckState(RApplicationSettings::getDefaultSoftwareCheckUpdates() ? Qt::Checked : Qt::Unchecked);
    this->onAiConsentClearClicked();
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

void RApplicationSettingsWidget::onDontUseNativeMenuBarChanged(Qt::CheckState state)
{
    this->applicationSettings->setDontUseNativeMenuBar(state == Qt::Checked);
    RMessageBox::information(this,tr("Menubar change"),tr("Menubar change requires an application restart."));
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

void RApplicationSettingsWidget::onCloudClientCertificateRenewChanged(Qt::CheckState state)
{
    this->applicationSettings->setCloudClientCertificateRenew(state == Qt::Checked);
    this->cloudClientCertificateExpiryDaysLabel->setEnabled(state == Qt::Checked);
    this->cloudClientCertificateExpiryDaysSpin->setEnabled(state == Qt::Checked);
}

void RApplicationSettingsWidget::onCloudClientCertificateExpiryDaysChanged(int cloudClientCertificateExpiryDays)
{
    this->applicationSettings->setCloudClientCertificateExpiryDays(cloudClientCertificateExpiryDays);
}

void RApplicationSettingsWidget::onSoftwareSendUsageInfoChanged(Qt::CheckState state)
{
    this->applicationSettings->setSoftwareSendUsageInfo(state == Qt::Checked);
}

void RApplicationSettingsWidget::onSoftwareCheckUpdatesChanged(Qt::CheckState state)
{
    this->applicationSettings->setSoftwareCheckUpdates(state == Qt::Checked);
}

void RApplicationSettingsWidget::onAiConsentClearClicked()
{
    this->applicationSettings->clearAiDataSharingConsent();
    this->aiConsentClearButton->setEnabled(false);
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
