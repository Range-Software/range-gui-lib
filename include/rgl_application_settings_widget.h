#ifndef RGL_APPLICATION_SETTINGS_WIDGET_H
#define RGL_APPLICATION_SETTINGS_WIDGET_H

#include <QTabWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>

#include <rcl_cloud_client.h>

#include "rgl_application_settings.h"
#include "rgl_file_chooser_button.h"
#include "rgl_keyboard_shortcuts_editor_widget.h"
#include "rgl_proxy_settings_widget.h"
#include "rgl_territory_combo_box.h"

class RApplicationSettingsWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Application settings.
        RApplicationSettings *applicationSettings;

        bool enableUserInfo;

        //! Tab widget.
        QTabWidget *tabWidget;

        QComboBox *styleCombo;
        QComboBox *languageCombo;
        QComboBox *formatCombo;
        RProxySettingsWidget *proxySettingsWidget;
        QSpinBox *cloudRefreshTimeoutSpin;
        QCheckBox *softwareSendUsageInfoCheckBox;
        QCheckBox *softwareCheckUpdatesCheckBox;
        QLineEdit *userFullNameEdit;
        QLineEdit *userEmailEdit;
        RTerritoryComboBox *userTerritoryCombo;
        RFileChooserButton *helpDirectoryButton;
        RFileChooserButton *opensslToolFileButton;
        RFileChooserButton *opensslConfFileButton;
        RKeyboardShortcutsEditorWidget *keyboardShortcutsEdirotWidget;

    public:

        //! Constructor.
        explicit RApplicationSettingsWidget(RApplicationSettings *applicationSettings, QWidget *parent = nullptr);

    public slots:

        //! Set default values.
        void setDefaultValues();

    protected slots:

        //! Style combo selection changed.
        void onStyleChanged(const QString &style);

        //! Language combo selection changed.
        void onLanguageChanged(const QString &languageName);

        //! Format combo selection changed.
        void onFormatChanged(const QString &formatName);

        //! Proxy changed.
        void onProxyChanged(const RHttpProxySettings &proxySettings);

        //! Cloud refresh timeout changed.
        void onCloudRefreshTimeoutChanged(int cloudRefreshTimeout);

        //! Send software usage information.
        void onSoftwareSendUsageInfoChanged(Qt::CheckState state);

        //! Check software updates.
        void onSoftwareCheckUpdatesChanged(Qt::CheckState state);

        //! User full name changed.
        void onUserFullNameChanged(const QString &userFullName);

        //! User email changed.
        void onUserEmailChanged(const QString &userEmail);

        //! User teritory selected.
        void onUserTerritorySelected(const QString &userTerritory);

        //! Help directory has changed.
        void onHelpDirectoryChanged(const QString &directory);

        //! OpenSSL tool file name changed.
        void onOpensslToolFileChanged(const QString &fileName);

        //! OpenSSL configuration file name changed.
        void onOpensslConfFileChanged(const QString &fileName);

        //! Keyboard shortcut changed.
        void onKeyboardShortcutChanged(uint position, const QString &shortcut);

};

#endif // RGL_APPLICATION_SETTINGS_WIDGET_H
