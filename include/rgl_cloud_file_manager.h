#ifndef RGL_CLOUD_FILE_MANAGER_H
#define RGL_CLOUD_FILE_MANAGER_H

#include <QTimer>

#include <rcl_file_manager.h>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"

class RCloudFileManager : public QObject
{
    Q_OBJECT

    protected:

        const RApplicationSettings *applicationSettings;

        RFileManager *fileManager;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        explicit RCloudFileManager(RCloudConnectionHandler *connectionHandler, const RApplicationSettings *applicationSettings, QObject *parent = nullptr);

    private:

        //! Create file manager settings from application settings.
        static RFileManagerSettings generateFileManaferSettings(const RApplicationSettings *applicationSettings);

        //! Restart the refresh timer.
        void restartRefreshTimer();

    private slots:

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

        //! Sync data directory has changed.
        void onSyncDataDirectoryChanged(bool syncDataDirectory);

    signals:

};

#endif // RGL_CLOUD_FILE_MANAGER_H
