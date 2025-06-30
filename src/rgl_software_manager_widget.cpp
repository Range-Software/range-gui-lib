#include <QVBoxLayout>
#include <QDir>
#include <QDesktopServices>
#include <QCheckBox>

#include <rbl_logger.h>
#include <rbl_utils.h>

#include <rcl_cloud_session_manager.h>

#include "rgl_message_box.h"
#include "rgl_software_manager_widget.h"

RSoftwareManagerWidget::RSoftwareManagerWidget(const RApplicationSettings *applicationSettings, QWidget *parent)
    : QWidget{parent}
    , showProgress{false}
{
    RCloudSessionInfo rangeSession = RCloudSessionManager::getDefaultRangeSession();

    RHttpClientSettings httpClientSettings;
    httpClientSettings.setUrl(RHttpClient::buildUrl(rangeSession.getHostName(),rangeSession.getPublicPort()));
    httpClientSettings.setTimeout(rangeSession.getTimeout());
    httpClientSettings.setTlsTrustStore(rangeSession.getHostCertificate());
    httpClientSettings.setProxySettings(applicationSettings->getProxySettings());

    RSoftwareManagerSettings softwareManagerSettings;
    softwareManagerSettings.setHttpClientSettings(httpClientSettings);
    softwareManagerSettings.setFileTags(QStringList());

    this->softwareManager = new RSoftwareManager(softwareManagerSettings,this);
    this->softwareManager->getCloudClient()->setBlocking(false);

    QObject::connect(applicationSettings,&RApplicationSettings::proxySettingsChanged,this,&RSoftwareManagerWidget::onProxySettingsChanged);

    this->progressDialog = new QProgressDialog(this);
    this->progressDialog->setRange(0,100);
    this->progressDialog->setMinimumDuration(1000);
    this->progressDialog->setCancelButtonText(tr("Cancel"));
    QObject::connect(this->progressDialog,&QProgressDialog::canceled,this->softwareManager->getCloudClient(),&RCloudClient::cancelTask);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QList<RTreeWidget::ColumnInfoItem> columnInfo;
    for (int type=ColumnId;type<NunberOfColumns;type++)
    {
        RTreeWidget::ColumnInfoItem columnInfoItem;
        columnInfoItem.column = type;
        columnInfoItem.title = RSoftwareManagerWidget::getColumnName(ColumnType(type));
        columnInfoItem.hidden = (type == ColumnId || type == ColumnAccessMode || type == ColumnAccessOwner || type == ColumnCreationDateTime || type == ColumnUpdateDateTime);
        columnInfoItem.available = true;
        columnInfo.append(columnInfoItem);
    }

    this->cloudFilesWidget = new RTreeWidget(columnInfo);
    mainLayout->addWidget(this->cloudFilesWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    this->allSoftwareCheckBox = new QCheckBox(RSoftwareManagerWidget::getAllSoftwareCheckBoxLabel(false));
    this->allSoftwareCheckBox->setCheckState(Qt::Unchecked);
    buttonsLayout->addWidget(this->allSoftwareCheckBox);

    buttonsLayout->addWidget(new QWidget, 1);

    QIcon downloadIcon(":/icons/cloud/pixmaps/range-download.svg");

    this->installButton = new QPushButton(downloadIcon,tr("Download and install"));
    this->installButton->setDisabled(true);
    buttonsLayout->addWidget(this->installButton);

    QObject::connect(this->cloudFilesWidget,&QTreeWidget::itemSelectionChanged,this,&RSoftwareManagerWidget::onItemSelectionChanged);

    QObject::connect(this->allSoftwareCheckBox,&QCheckBox::toggled,this,&RSoftwareManagerWidget::onAllSoftwareBoxToggled);
    QObject::connect(this->installButton,&QPushButton::clicked,this,&RSoftwareManagerWidget::onInstallClicked);

    QObject::connect(this->softwareManager,&RSoftwareManager::softwareAvailable,this,&RSoftwareManagerWidget::onSoftwareAvailable);
    QObject::connect(this->softwareManager->getCloudClient(),&RCloudClient::fileDownloaded,this,&RSoftwareManagerWidget::onSoftwareDownloaded);
    QObject::connect(this->softwareManager->getCloudClient(),&RCloudClient::downloadProgress,this,&RSoftwareManagerWidget::onDownloadProgress);
    QObject::connect(this->softwareManager->getCloudClient(),&RCloudClient::submitted,this,&RSoftwareManagerWidget::onClientSubmitted);
    QObject::connect(this->softwareManager->getCloudClient(),&RCloudClient::finished,this,&RSoftwareManagerWidget::onClientFinished);
    QObject::connect(this->softwareManager->getCloudClient(),&RCloudClient::failed,this,&RSoftwareManagerWidget::onClientFailed);

    this->refreshCloudFiles();
}

void RSoftwareManagerWidget::refreshCloudFiles()
{
    try
    {
        this->showProgress = false;
        this->softwareManager->checkForUpdates();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of files from cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("List of files from Cloud has failed."));
    }
}

QString RSoftwareManagerWidget::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnId:               return tr("ID");
        case ColumnName:             return tr("Name");
        case ColumnSize:             return tr("Size");
        case ColumnCreationDateTime: return tr("Creation time");
        case ColumnUpdateDateTime:   return tr("Update time");
        case ColumnAccessMode:       return tr("Access rights");
        case ColumnAccessOwner:      return tr("Owner");
        case ColumnVersion:          return tr("Version");
        case ColumnTags:             return tr("Tags");
        case ColumnMd5Checksum:      return tr("Checksum");
        default:                     return QString();
    }
}

void RSoftwareManagerWidget::populateFiles(const QList<RFileInfo> &fileInfoList, QTreeWidget *treeWidget)
{
    QList<RFileInfo> _fileInfoList = fileInfoList;

    for (int i=treeWidget->topLevelItemCount()-1; i>=0; i--)
    {
        const QUuid fileId = QUuid(treeWidget->topLevelItem(i)->text(ColumnId));
        bool fileInList = false;
        for (auto iter = _fileInfoList.cbegin(), end = _fileInfoList.cend(); iter != end; ++iter)
        {
            if (fileId == iter->getId())
            {
                RSoftwareManagerWidget::updateFileListItem(treeWidget->topLevelItem(i),*iter);
                _fileInfoList.erase(iter);
                fileInList = true;
                break;
            }
        }
        if (!fileInList)
        {
            treeWidget->takeTopLevelItem(i);
        }
    }
    for (const RFileInfo &fileInfo : _fileInfoList)
    {
        RSoftwareManagerWidget::updateFileListItem(new QTreeWidgetItem(treeWidget),fileInfo);
    }

    treeWidget->resizeColumnToContents(ColumnName);
}

void RSoftwareManagerWidget::updateFileListItem(QTreeWidgetItem *listItem, const RFileInfo &fileInfo)
{
    listItem->setText(ColumnId,fileInfo.getId().toString(QUuid::StringFormat::WithoutBraces));
    listItem->setData(ColumnId,Qt::UserRole,QVariant(fileInfo.getId()));
    listItem->setText(ColumnName,fileInfo.getPath());
    listItem->setText(ColumnSize,RUtil::readableBytes(fileInfo.getSize()));
    listItem->setText(ColumnCreationDateTime,QDateTime::fromSecsSinceEpoch(fileInfo.getCreationDateTime()).toString());
    listItem->setText(ColumnUpdateDateTime,QDateTime::fromSecsSinceEpoch(fileInfo.getUpdateDateTime()).toString());
    listItem->setText(ColumnAccessMode,fileInfo.getAccessRights().getMode().toHuman());
    listItem->setText(ColumnAccessOwner,fileInfo.getAccessRights().getOwner().toString(':'));
    listItem->setText(ColumnVersion,fileInfo.getVersion().toString());
    listItem->setText(ColumnTags,fileInfo.getTags().join(','));
    listItem->setText(ColumnMd5Checksum,fileInfo.getMd5Checksum());
}

QStringList RSoftwareManagerWidget::getExpectedTags(bool allSoftwareChecked)
{
    if (allSoftwareChecked)
    {
        return QStringList();
    }
    else
    {
        return RSoftwareManager::getExpectedTags();
    }
}

QString RSoftwareManagerWidget::getAllSoftwareCheckBoxLabel(bool allSoftwareChecked)
{
    return tr("Show all available software") + " (" + tr("Expected tags") + ": " + RSoftwareManagerWidget::getExpectedTags(allSoftwareChecked).join(", ") + ")";
}

void RSoftwareManagerWidget::onItemSelectionChanged()
{
    this->installButton->setDisabled(this->cloudFilesWidget->selectedItems().empty());
}

void RSoftwareManagerWidget::onAllSoftwareBoxToggled(bool checked)
{
    this->allSoftwareCheckBox->setText(RSoftwareManagerWidget::getAllSoftwareCheckBoxLabel(checked));

    QStringList expectedTags = RSoftwareManagerWidget::getExpectedTags(checked);
    for (int i=this->cloudFilesWidget->topLevelItemCount()-1; i>=0; i--)
    {
        QStringList tags = this->cloudFilesWidget->topLevelItem(i)->text(ColumnTags).split(',');

        bool itemIsHidden = false;
        for (const QString &expectedTag : std::as_const(expectedTags))
        {
            if (!tags.contains(expectedTag))
            {
                itemIsHidden = true;
                break;
            }
        }
        this->cloudFilesWidget->topLevelItem(i)->setHidden(itemIsHidden);
    }
}

void RSoftwareManagerWidget::onInstallClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->cloudFilesWidget->selectedItems();

    for (const QTreeWidgetItem *item : std::as_const(selectedItems))
    {
        QString fileName(item->text(ColumnName));
        QUuid id(item->data(ColumnId,Qt::UserRole).toUuid());

        QString path(QDir(RApplicationSettings::getTmpDir()).absoluteFilePath(fileName));

        this->softwareManager->downloadFile(path,id);
    }
}

void RSoftwareManagerWidget::onSoftwareAvailable(QList<RFileInfo> fileInfoList)
{
    RSoftwareManagerWidget::populateFiles(fileInfoList,this->cloudFilesWidget);
    this->onAllSoftwareBoxToggled(this->allSoftwareCheckBox->isChecked());
}

void RSoftwareManagerWidget::onSoftwareDownloaded(QString fileName)
{
    QString question = tr("Would you like to install downloaded software file?")
                     + "<br/><br/>" + tr("File") + ": <tt>" + fileName + "</tt>";
    if (RMessageBox::question(this,tr("Install software?"),question) == RMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl("file://" + fileName, QUrl::TolerantMode));
    }
}

void RSoftwareManagerWidget::onProxySettingsChanged(RHttpProxySettings proxySettings)
{
    RSoftwareManagerSettings softwareManagerSettings = this->softwareManager->getSoftwareManagerSettings();
    RHttpClientSettings httpClientSettings = softwareManagerSettings.getHttpClientSettings();
    httpClientSettings.setProxySettings(proxySettings);
    softwareManagerSettings.setHttpClientSettings(httpClientSettings);
    this->softwareManager->setSoftwareManagerSettings(softwareManagerSettings);
}

void RSoftwareManagerWidget::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0)
    {
        this->progressDialog->setRange(0,bytesTotal+1);
        this->progressDialog->setValue(bytesReceived);
    }
}

void RSoftwareManagerWidget::onClientSubmitted()
{
    this->progressDialog->setValue(this->progressDialog->minimum());
}

void RSoftwareManagerWidget::onClientFinished()
{
    this->progressDialog->setValue(this->progressDialog->maximum());
}

void RSoftwareManagerWidget::onClientFailed()
{
    this->progressDialog->setValue(this->progressDialog->maximum());
}
