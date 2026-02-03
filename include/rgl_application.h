#ifndef RGL_APPLICATION_H
#define RGL_APPLICATION_H

#include <QApplication>
#include <QLockFile>
#include <QMainWindow>
#include <QStringList>

#include <rcl_cloud_session_manager.h>

#include "rgl_application_settings.h"
#include "rgl_action_list.h"
#include "rgl_cloud_connection_handler.h"

class RApplication : public QApplication
{

    Q_OBJECT

    protected:

        //! Lock file.
        QLockFile *pLockFile;
        //! Cloud session manager.
        RCloudSessionManager *cloudSessionManager;
        //! Cloud connection handler.
        RCloudConnectionHandler *cloudConnectionHandler;
        //! Application settings.
        RApplicationSettings *applicationSettings;
        //! List of actions.
        RActionList *actionList;
        //! Main window.
        QMainWindow *mainWindow;

        //! File updater source directories.
        QStringList updaterSourceDirs;

        //! Files to be loaded.
        QStringList filesToLoad;
        //! Application information;
        QStringList userAppInfo;

        //! Additional translators.
        QStringList translatorNames;

    public:

        //! Constructor.
        explicit RApplication(int &argc, char **argv, bool initializeSettings = true);

        //! Return application instance.
        static RApplication *instance() noexcept;

        //! Return const reference to cloud session manager.
        const RCloudSessionManager *getCloudSessionManager() const;

        //! Return reference to cloud session manager.
        RCloudSessionManager *getCloudSessionManager();

        //! Return pointer to cloud connection handler.
        RCloudConnectionHandler *getCloudConnectionHandler();

        //! Return const pointer to application settings.
        const RApplicationSettings *getApplicationSettings() const;

        //! Return pointer to application settings.
        RApplicationSettings *getApplicationSettings();

        //! Return const pointer to action list.
        const RActionList *getActionList() const;

        //! Return pointer to action list.
        RActionList *getActionList();

        //! Return const pointer to main window.
        const QMainWindow *getMainWindow() const;

        //! Return pointer to main window.
        QMainWindow *getMainWindow();

        //! Apply translation.
        void applyTranslation(const QString &languageCode);

        //! Apply format.
        void applyFormat(RApplicationSettings::Format format);

        //! Initialize main window and other user objects.
        virtual void initialize() = 0;

        //! Initialize main window and other user objects.
        virtual void finalize() = 0;

    private:

        //! Update files
        void updateFiles();

        //! Check cloud client certificate validity.
        void validateCloudClientCertificate();

        //! Send software usage report.
        void sendUsageReport(const QString &logFile) const;

    signals:

        //! Application has started.
        void started();

    protected slots:

        //! Catch started signal.
        void onStarted();

        //! Catch aboutToQuit signal.
        void onAboutToQuit();

        //! Catch style changed signal.
        void onStyleChanged(const QString &styleName);

        //! Catch color scheme changed signal.
        void onColorSchemeChanged(Qt::ColorScheme colorScheme);

        //! Catch language changed signal.
        void onLanguageChanged(const QString &languageCode);

        //! Catch format changed signal.
        void onFormatChanged(RApplicationSettings::Format format);

        //! Software is available.
        void onSoftwareAvailable(QList<RFileInfo> fileInfoList);

        //! OpenSSL CSR task has finished.
        void onOpenSslCsrTaskFinished(RCloudSessionInfo sessionInfo);

        //! OpenSSL CSR task has failed.
        void onOpenSslCsrTaskFailed(QString message);

};

#endif // RGL_APPLICATION_H
