#ifndef RGL_SOFTWARE_UPDATE_CHECKER_H
#define RGL_SOFTWARE_UPDATE_CHECKER_H

#include <QObject>
#include <QTimer>

#include <rcl_software_manager.h>

#include "rgl_application_settings.h"

class RSoftwareUpdateChecker : public QObject
{
    Q_OBJECT

    protected:

        QList<RFileInfo> lastFileInfoList;

        RSoftwareManager *softwareManager;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        explicit RSoftwareUpdateChecker(const RApplicationSettings *applicationSettings, QObject *parent = nullptr);

    protected:

        //! Trigger request to list files.
        void refreshCloudFiles();

        //! Check is file info lists are equal.
        static bool fileInfoListsEqual(const QList<RFileInfo> &a, const QList<RFileInfo> &b);

    protected slots:

        //! Software is available.
        void onSoftwareAvailable(QList<RFileInfo> fileInfoList);

        //! Proxy settings have changed.
        void onProxySettingsChanged(RHttpProxySettings proxySettings);

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

    signals:

        void softwareAvailable(QList<RFileInfo> fileInfoList);

};

#endif // RGL_SOFTWARE_UPDATE_CHECKER_H
