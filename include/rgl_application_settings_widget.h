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

class RApplicationSettingsWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Application settings.
        RApplicationSettings *applicationSettings;

        bool enablePersonalInfo;

        //! Tab widget.
        QTabWidget *tabWidget;

        QComboBox *styleCombo;
        QComboBox *languageCombo;
        QComboBox *formatCombo;
        RProxySettingsWidget *proxySettingsWidget;
        QSpinBox *cloudRefreshTimeoutSpin;
        QCheckBox *softwareSendUsageInfoCheckBox;
        QCheckBox *softwareCheckUpdatesCheckBox;
        QLineEdit *personalNameEdit;
        QLineEdit *personalEmailEdit;
        RFileChooserButton *helpDirectoryButton;
        RFileChooserButton *opensslToolFileButton;
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

        //! Personal name changed.
        void onPersonalNameChanged(const QString &personalName);

        //! Personal email changed.
        void onPersonalEmailChanged(const QString &personalEmail);

        //! Help directory has changed.
        void onHelpDirectoryChanged(const QString &directory);

        //! OpenSSL tool file name changed.
        void onOpensslToolFileChanged(const QString &fileName);

        //! Keyboard shortcut changed.
        void onKeyboardShortcutChanged(uint position, const QString &shortcut);

};

#endif // RGL_APPLICATION_SETTINGS_WIDGET_H
