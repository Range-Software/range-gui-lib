#include <rbl_logger.h>
#include <rbl_utils.h>

#include <rcl_cloud_session_manager.h>

#include "rgl_cloud_file_manager.h"

RCloudFileManager::RCloudFileManager(RCloudConnectionHandler *connectionHandler, const RApplicationSettings *applicationSettings, QObject *parent)
    : QObject{parent}
    , applicationSettings{applicationSettings}
{
    RCloudClient *cloudClient = connectionHandler->createPrivateClient(this);
    cloudClient->setBlocking(false);

    this->fileManager = new RFileManager(RCloudFileManager::generateFileManaferSettings(this->applicationSettings),cloudClient,this);

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&RCloudFileManager::onRefreshTimeout);

    QObject::connect(this->applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&RCloudFileManager::onRefreshTimeoutChanged);
    QObject::connect(this->applicationSettings,&RApplicationSettings::cloudSyncDataDirectoryChanged,this,&RCloudFileManager::onSyncDataDirectoryChanged);

    this->restartRefreshTimer();
}

RFileManagerSettings RCloudFileManager::generateFileManaferSettings(const RApplicationSettings *applicationSettings)
{
    RFileManagerSettings fileManagerSetting;
    fileManagerSetting.setFileTags({RVendor::packageName()});
    fileManagerSetting.setLocalDirectory(applicationSettings->getDataDir());
    fileManagerSetting.setCacheFile(applicationSettings->getCloudSyncDataCachePath());
    return fileManagerSetting;
}

void RCloudFileManager::restartRefreshTimer()
{
    if (this->applicationSettings->getCloudRefreshTimeout() > 0 && this->applicationSettings->getCloudSyncDataDirectory())
    {
        RLogger::info("Start cloud files refres timer\n");
        this->refreshTimer->start(this->applicationSettings->getCloudRefreshTimeout());
    }
    else
    {
        if (this->refreshTimer->isActive())
        {
            RLogger::info("Stop cloud files refres timer\n");
            this->refreshTimer->stop();
        }
    }
}

void RCloudFileManager::onRefreshTimeout()
{
    if (this->fileManager->getNRunningActions() > 0)
    {
        RLogger::warning("Cannot sync files because previous sync has not yet finished (running actions = %u)).\n",this->fileManager->getNRunningActions());
    }
    else
    {
        RLogger::info("Refresh cloud files list\n");
        this->fileManager->requestSyncFiles();
    }
}

void RCloudFileManager::onRefreshTimeoutChanged(uint refreshTimeout)
{
    this->restartRefreshTimer();
}

void RCloudFileManager::onSyncDataDirectoryChanged(bool syncDataDirectory)
{
    if (syncDataDirectory)
    {
        this->onRefreshTimeout();
    }
    this->restartRefreshTimer();
}
