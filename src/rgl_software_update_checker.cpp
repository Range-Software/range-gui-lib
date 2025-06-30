#include <rbl_logger.h>
#include <rbl_utils.h>

#include <rcl_cloud_session_manager.h>

#include "rgl_software_update_checker.h"

RSoftwareUpdateChecker::RSoftwareUpdateChecker(const RApplicationSettings *applicationSettings, QObject *parent)
    : QObject{parent}
{
    RCloudSessionInfo rangeSession = RCloudSessionManager::getDefaultRangeSession();

    RHttpClientSettings httpClientSettings;
    httpClientSettings.setUrl(RHttpClient::buildUrl(rangeSession.getHostName(),rangeSession.getPublicPort()));
    httpClientSettings.setTimeout(rangeSession.getTimeout());
    httpClientSettings.setTlsTrustStore(rangeSession.getHostCertificate());
    httpClientSettings.setProxySettings(applicationSettings->getProxySettings());

    RSoftwareManagerSettings softwareManagerSettings;
    softwareManagerSettings.setHttpClientSettings(httpClientSettings);
    softwareManagerSettings.setFileTags(RSoftwareManager::getExpectedTags());

    this->softwareManager = new RSoftwareManager(softwareManagerSettings,this);
    this->softwareManager->getCloudClient()->setBlocking(false);

    QObject::connect(applicationSettings,&RApplicationSettings::proxySettingsChanged,this,&RSoftwareUpdateChecker::onProxySettingsChanged);
    QObject::connect(applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&RSoftwareUpdateChecker::onRefreshTimeoutChanged);
    QObject::connect(this->softwareManager,&RSoftwareManager::softwareAvailable,this,&RSoftwareUpdateChecker::onSoftwareAvailable);

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&RSoftwareUpdateChecker::onRefreshTimeout);
    this->refreshTimer->start(applicationSettings->getCloudRefreshTimeout());
    this->onRefreshTimeoutChanged(applicationSettings->getCloudRefreshTimeout());

    this->refreshCloudFiles();
}

void RSoftwareUpdateChecker::refreshCloudFiles()
{
    try
    {
        this->softwareManager->checkForUpdates();
    }
    catch (const RError &rError)
    {
        RLogger::error("Software update check has failed. %s\n",rError.getMessage().toUtf8().constData());
    }
}

bool RSoftwareUpdateChecker::fileInfoListsEqual(const QList<RFileInfo> &a, const QList<RFileInfo> &b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    for (qsizetype i=0;i<a.size();i++)
    {
        if (a.at(i).getId() != b.at(i).getId() ||
            a.at(i).getPath() != b.at(i).getPath() ||
            a.at(i).getVersion() != b.at(i).getVersion() ||
            a.at(i).getMd5Checksum() != b.at(i).getMd5Checksum())
        {
            return false;
        }
    }

    return true;
}

void RSoftwareUpdateChecker::onSoftwareAvailable(QList<RFileInfo> fileInfoList)
{
    fileInfoList.removeIf(
        [](const RFileInfo &fileInfo)
        {
            return fileInfo.getVersion() <= RVendor::version();
        }
        );

    struct
    {
        bool operator()(const RFileInfo &a, const RFileInfo &b) const { return a.getId() < b.getId(); }
    }
    customLess;

    std::sort(fileInfoList.begin(), fileInfoList.end(), customLess);

    if (RSoftwareUpdateChecker::fileInfoListsEqual(this->lastFileInfoList, fileInfoList))
    {
        return;
    }

    this->lastFileInfoList = fileInfoList;

    if (!this->lastFileInfoList.isEmpty())
    {
        emit this->softwareAvailable(fileInfoList);
    }
}

void RSoftwareUpdateChecker::onProxySettingsChanged(RHttpProxySettings proxySettings)
{
    RSoftwareManagerSettings softwareManagerSettings = this->softwareManager->getSoftwareManagerSettings();
    RHttpClientSettings httpClientSettings = softwareManagerSettings.getHttpClientSettings();
    httpClientSettings.setProxySettings(proxySettings);
    softwareManagerSettings.setHttpClientSettings(httpClientSettings);
    this->softwareManager->setSoftwareManagerSettings(softwareManagerSettings);
}

void RSoftwareUpdateChecker::onRefreshTimeout()
{
    this->refreshCloudFiles();
}

void RSoftwareUpdateChecker::onRefreshTimeoutChanged(uint refreshTimeout)
{
    if (refreshTimeout == 0)
    {
        this->refreshTimer->stop();
    }
    else
    {
        this->refreshTimer->start(refreshTimeout);
    }
}
