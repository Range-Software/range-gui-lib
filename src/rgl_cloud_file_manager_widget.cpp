#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QMenu>
#include <QDir>
#include <QTimer>
#include <QLocale>

#include <rbl_logger.h>
#include <rbl_utils.h>

#include "rgl_access_owner_dialog.h"
#include "rgl_access_mode_dialog.h"
#include "rgl_file_chooser_button.h"
#include "rgl_file_version_dialog.h"
#include "rgl_file_tags_dialog.h"
#include "rgl_cloud_file_manager_widget.h"
#include "rgl_message_box.h"
#include "rgl_progress_dialog.h"

RCloudFileManagerWidget::RCloudFileManagerWidget(RCloudConnectionHandler *connectionHandler,
                                                 RApplicationSettings *applicationSettings,
                                                 bool allowDirectorySelector,
                                                 QWidget *parent)
    : QWidget{parent}
    , applicationSettings{applicationSettings}
    , allowDirectorySelector{allowDirectorySelector}
{
    this->localDirectoryPath = this->applicationSettings->getDataDir();

    this->cloudClient = connectionHandler->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QIcon removeIcon(":/icons/cloud/pixmaps/range-remove.svg");
    QIcon refreshIcon(":/icons/cloud/pixmaps/range-refresh.svg");
    QIcon downloadIcon(":/icons/cloud/pixmaps/range-download.svg");
    QIcon uploadIcon(":/icons/cloud/pixmaps/range-upload.svg");
    QIcon updateIcon(":/icons/cloud/pixmaps/range-upload_update.svg");
    QIcon accessRightsIcon(":/icons/cloud/pixmaps/range-access_rights.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *verticalSplitter = new QSplitter(Qt::Horizontal);
    verticalSplitter->setChildrenCollapsible(false);
    mainLayout->addWidget(verticalSplitter);

    // Local

    QGroupBox *localWidget = new QGroupBox(tr("Local"));
    verticalSplitter->addWidget(localWidget);

    QVBoxLayout *localLayout = new QVBoxLayout;
    localWidget->setLayout(localLayout);

    if (this->allowDirectorySelector)
    {
        RFileChooserButton *localDirectoryChooser = new RFileChooserButton(tr("Directory"),RFileChooserButton::Directory);
        localDirectoryChooser->hideClearButton();
        localDirectoryChooser->setFileName(this->localDirectoryPath);
        localLayout->addWidget(localDirectoryChooser);

        QObject::connect(localDirectoryChooser,&RFileChooserButton::fileNameChanged,this,&RCloudFileManagerWidget::onLocalDirectoryChanged);
    }

    QList<RTreeWidget::ColumnInfoItem> localColumnInfo;
    for (int type=ColumnId;type<NunberOfColumns;type++)
    {
        RTreeWidget::ColumnInfoItem columnInfoItem;
        columnInfoItem.column = type;
        columnInfoItem.title = RCloudFileManagerWidget::getColumnName(ColumnType(type));
        columnInfoItem.hidden = (type == ColumnId || type == ColumnCreationDateTime || type == ColumnAccessMode || type == ColumnAccessOwner || type == ColumnVersion || type == ColumnTags);
        columnInfoItem.available = !columnInfoItem.hidden;
        localColumnInfo.append(columnInfoItem);
    }

    this->localFilesWidget = new RTreeWidget(localColumnInfo);
    this->localFilesWidget->setSelectionMode(QTreeWidget::ExtendedSelection);
    RCloudFileManagerWidget::populateLocalFiles(this->localDirectoryPath,this->localFilesWidget);
    localLayout->addWidget(this->localFilesWidget);

    this->localFileSystemWatcher = new QFileSystemWatcher;
    if (!this->localFileSystemWatcher->addPath(this->localDirectoryPath))
    {
        RLogger::warning("Could not add path \'%s\' to file system watcher\n",this->localDirectoryPath.toUtf8().constData());
    }

    QObject::connect(this->localFileSystemWatcher,&QFileSystemWatcher::directoryChanged,this,&RCloudFileManagerWidget::onLocalDirectoryChanged);

    QHBoxLayout *localButtonsLayout = new QHBoxLayout;
    localLayout->addLayout(localButtonsLayout);

    this->localDeleteButton = new QPushButton(removeIcon,tr("Delete"));
    this->localDeleteButton->setDisabled(true);
    localButtonsLayout->addWidget(this->localDeleteButton);

    localButtonsLayout->addWidget(new QWidget, 1);

    this->localUploadButton = new QPushButton(uploadIcon,tr("Upload"));
    this->localUploadButton->setDisabled(true);
    localButtonsLayout->addWidget(this->localUploadButton);

    this->localUpdateButton = new QPushButton(updateIcon,tr("Update"));
    this->localUpdateButton->setDisabled(true);
    localButtonsLayout->addWidget(this->localUpdateButton);

    QObject::connect(this->localUploadButton,&QPushButton::clicked,this,&RCloudFileManagerWidget::onLocalUploadButtonClicked);
    QObject::connect(this->localUpdateButton,&QPushButton::clicked,this,&RCloudFileManagerWidget::onLocalUpdateButtonClicked);
    QObject::connect(this->localDeleteButton,&QPushButton::clicked,this,&RCloudFileManagerWidget::onLocalDeleteButtonClicked);

    // Cloud

    QGroupBox *cloudWidget = new QGroupBox(tr("Cloud"));
    verticalSplitter->addWidget(cloudWidget);

    QVBoxLayout *cloudLayout = new QVBoxLayout;
    cloudWidget->setLayout(cloudLayout);

    QList<RTreeWidget::ColumnInfoItem> cloudColumnInfo;
    for (int type=ColumnId;type<NunberOfColumns;type++)
    {
        RTreeWidget::ColumnInfoItem columnInfoItem;
        columnInfoItem.column = type;
        columnInfoItem.title = RCloudFileManagerWidget::getColumnName(ColumnType(type));
        columnInfoItem.hidden = false;
        columnInfoItem.available = true;
        cloudColumnInfo.append(columnInfoItem);
    }

    this->cloudFilesWidget = new RTreeWidget(cloudColumnInfo);
    this->cloudFilesWidget->setSelectionMode(QTreeWidget::ExtendedSelection);
    cloudLayout->addWidget(this->cloudFilesWidget);

    QHBoxLayout *cloudButtonsLayout = new QHBoxLayout;
    cloudLayout->addLayout(cloudButtonsLayout);

    this->cloudDownloadButton = new QPushButton(downloadIcon,tr("Download"));
    this->cloudDownloadButton->setDisabled(true);
    cloudButtonsLayout->addWidget(this->cloudDownloadButton);

    this->cloudEditButton = new QPushButton(accessRightsIcon,tr("Edit"));
    this->cloudEditButton->setDisabled(true);
    cloudButtonsLayout->addWidget(this->cloudEditButton);

    QMenu *cloudEditMenu = new QMenu;
    this->cloudEditButton->setMenu(cloudEditMenu);

    QAction *cloudAccessModeAction = new QAction(tr("Access mode"));
    cloudEditMenu->addAction(cloudAccessModeAction);

    QAction *cloudAccessOwnerAction = new QAction(tr("Owner"));
    cloudEditMenu->addAction(cloudAccessOwnerAction);

    QAction *cloudVersionAction = new QAction(tr("Version"));
    cloudEditMenu->addAction(cloudVersionAction);

    QAction *cloudTagsAction = new QAction(tr("Tags"));
    cloudEditMenu->addAction(cloudTagsAction);

    cloudButtonsLayout->addWidget(new QWidget, 1);

    this->cloudRefreshButton = new QPushButton(refreshIcon,tr("Refresh"));
    cloudButtonsLayout->addWidget(this->cloudRefreshButton);

    cloudButtonsLayout->addWidget(new QWidget, 1);

    this->cloudDeleteButton = new QPushButton(removeIcon,tr("Delete"));
    this->cloudDeleteButton->setDisabled(true);
    cloudButtonsLayout->addWidget(this->cloudDeleteButton);

    QObject::connect(this->localFilesWidget,&QTreeWidget::itemSelectionChanged,this,&RCloudFileManagerWidget::onLocalFileSelectionChanged);
    QObject::connect(this->cloudFilesWidget,&QTreeWidget::itemSelectionChanged,this,&RCloudFileManagerWidget::onCloudFileSelectionChanged);

    QObject::connect(this->cloudRefreshButton,&QPushButton::clicked,this,&RCloudFileManagerWidget::onCloudRefreshButtonClicked);
    QObject::connect(this->cloudDeleteButton,&QPushButton::clicked,this,&RCloudFileManagerWidget::onCloudDeleteButtonClicked);
    QObject::connect(this->cloudDownloadButton,&QPushButton::clicked,this,&RCloudFileManagerWidget::onCloudDownloadButtonClicked);

    QObject::connect(cloudAccessModeAction,&QAction::triggered,this,&RCloudFileManagerWidget::onCloudAccessModeActionTriggered);
    QObject::connect(cloudAccessOwnerAction,&QAction::triggered,this,&RCloudFileManagerWidget::onCloudAccessOwnerActionTriggered);
    QObject::connect(cloudVersionAction,&QAction::triggered,this,&RCloudFileManagerWidget::onCloudVersionActionTriggered);
    QObject::connect(cloudTagsAction,&QAction::triggered,this,&RCloudFileManagerWidget::onCloudTagsActionTriggered);

    QObject::connect(this->cloudClient,&RCloudClient::configurationChanged,this,&RCloudFileManagerWidget::onClientConfigurationChanged);
    QObject::connect(this->cloudClient,&RCloudClient::fileUploaded,this,&RCloudFileManagerWidget::onFileUploaded);
    QObject::connect(this->cloudClient,&RCloudClient::fileUpdated,this,&RCloudFileManagerWidget::onFileUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileAccessModeUpdated,this,&RCloudFileManagerWidget::onFileAccessModeUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileAccessOwnerUpdated,this,&RCloudFileManagerWidget::onFileAccessOwnerUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileVersionUpdated,this,&RCloudFileManagerWidget::onFileVersionUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileTagsUpdated,this,&RCloudFileManagerWidget::onFileTagsUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::fileDownloaded,this,&RCloudFileManagerWidget::onFileDownloaded);
    QObject::connect(this->cloudClient,&RCloudClient::fileListAvailable,this,&RCloudFileManagerWidget::onFileListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::fileRemoved,this,&RCloudFileManagerWidget::onFileRemoved);
    QObject::connect(this->cloudClient,&RCloudClient::submitted,this,&RCloudFileManagerWidget::onClientSubmitted);
    QObject::connect(this->cloudClient,&RCloudClient::finished,this,&RCloudFileManagerWidget::onClientFinished);
    QObject::connect(this->cloudClient,&RCloudClient::failed,this,&RCloudFileManagerWidget::onClientFailed);

    QObject::connect(applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&RCloudFileManagerWidget::onRefreshTimeoutChanged);

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&RCloudFileManagerWidget::onRefreshTimeout);
    this->refreshTimer->start(this->applicationSettings->getCloudRefreshTimeout());
    this->onRefreshTimeoutChanged(this->applicationSettings->getCloudRefreshTimeout());

    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::populateLocalFiles(const QString &path, QTreeWidget *treeWidget)
{
    QFileInfoList _fileInfoList = QDir(path).entryInfoList(QDir::Files | QDir::NoDotAndDotDot,QDir::Name);

    for (int i=treeWidget->topLevelItemCount()-1; i>=0; i--)
    {
        const QString &fileName = treeWidget->topLevelItem(i)->text(ColumnName);
        bool fileInList = false;
        for (auto iter = _fileInfoList.cbegin(), end = _fileInfoList.cend(); iter != end; ++iter)
        {
            if (fileName == iter->fileName())
            {
                RCloudFileManagerWidget::updateFileListItem(treeWidget->topLevelItem(i),*iter);
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

    for (const QFileInfo &fileInfo : std::as_const(_fileInfoList))
    {
        RCloudFileManagerWidget::updateFileListItem(new QTreeWidgetItem(treeWidget),fileInfo);
    }

    for (int type=ColumnName;type<NunberOfColumns;type++)
    {
        treeWidget->resizeColumnToContents(type);
    }
}

void RCloudFileManagerWidget::populateCloudFiles(const QList<RFileInfo> &fileInfoList, QTreeWidget *treeWidget)
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
                RCloudFileManagerWidget::updateFileListItem(treeWidget->topLevelItem(i),*iter);
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
        RCloudFileManagerWidget::updateFileListItem(new QTreeWidgetItem(treeWidget),fileInfo);
    }

    for (int type=ColumnName;type<NunberOfColumns;type++)
    {
        treeWidget->resizeColumnToContents(type);
    }
}

void RCloudFileManagerWidget::updateFileListItem(QTreeWidgetItem *listItem, const QFileInfo &fileInfo)
{
    listItem->setText(ColumnName,fileInfo.fileName());
    listItem->setText(ColumnSize,RUtil::readableBytes(fileInfo.size()));
    listItem->setData(ColumnUpdateDateTime,Qt::DisplayRole,fileInfo.lastModified());
    listItem->setText(ColumnMd5Checksum,RFileInfo::findMd5Checksum(fileInfo.filePath()));
}

void RCloudFileManagerWidget::updateFileListItem(QTreeWidgetItem *listItem, const RFileInfo &fileInfo)
{
    listItem->setText(ColumnId,fileInfo.getId().toString(QUuid::StringFormat::WithoutBraces));
    listItem->setData(ColumnId,Qt::UserRole,QVariant(fileInfo.getId()));
    listItem->setText(ColumnName,fileInfo.getPath());
    listItem->setText(ColumnSize,RUtil::readableBytes(fileInfo.getSize()));
    listItem->setData(ColumnCreationDateTime,Qt::DisplayRole,QDateTime::fromSecsSinceEpoch(fileInfo.getCreationDateTime()));
    listItem->setData(ColumnUpdateDateTime,Qt::DisplayRole,QDateTime::fromSecsSinceEpoch(fileInfo.getUpdateDateTime()));
    listItem->setText(ColumnAccessMode,fileInfo.getAccessRights().getMode().toHuman());
    listItem->setText(ColumnAccessOwner,fileInfo.getAccessRights().getOwner().toString(':'));
    listItem->setText(ColumnVersion,fileInfo.getVersion().toString());
    listItem->setText(ColumnTags,fileInfo.getTags().join(','));
    listItem->setText(ColumnMd5Checksum,fileInfo.getMd5Checksum());
}

QString RCloudFileManagerWidget::getColumnName(ColumnType columnType)
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

void RCloudFileManagerWidget::refreshCloudFiles()
{
    try
    {
        this->cloudClient->requestListFiles();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of files from cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("List of files from Cloud has failed."));
    }
}

void RCloudFileManagerWidget::uploadSelectedLocalFiles()
{
    QDir localDirectory(this->localDirectoryPath);

    QList<QTreeWidgetItem*> selectedItems = this->localFilesWidget->selectedItems();

    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        QString _fileName = selectedItem->text(ColumnName);
        try
        {
            RProgressDialog *progressDialog = new RProgressDialog(tr("File upload") + ": <tt>" + _fileName + "</tt>",tr("Cancel"),0,100,this);
            progressDialog->setJob(this->cloudClient->requestFileUpload(localDirectory.absoluteFilePath(selectedItem->text(ColumnName)),selectedItem->text(ColumnName)));

        }
        catch (const RError &rError)
        {
            RLogger::error("Failed to request file upload to Cloud. %s\n",rError.getMessage().toUtf8().constData());
            RMessageBox::critical(this,tr("File upload failed"),tr("File upload to Cloud has failed."));
        }
    }
}

void RCloudFileManagerWidget::updateSelectedLocalFiles()
{
    QDir localDirectory(this->localDirectoryPath);

    QList<QTreeWidgetItem*> localSelectedItems = this->localFilesWidget->selectedItems();
    QList<QTreeWidgetItem*> cloudSelectedItems = this->cloudFilesWidget->selectedItems();

    if (localSelectedItems.count() == 0)
    {
        RMessageBox::warning(this,tr("File update"),tr("Select a local file to be updated."));
        return;
    }
    if (cloudSelectedItems.count() == 0)
    {
        RMessageBox::warning(this,tr("File update"),tr("Select a Cloud file to be updated."));
        return;
    }

    QString question = tr("Are you sure you want to update selected Cloud file with selected local file?")
                     + "<ul>"
                     + "<li>" + tr("Local") + ": <tt>" + localSelectedItems.at(0)->text(ColumnName) + "</tt></li>"
                     + "<li>" + tr("Cloud") + ": <tt>" + cloudSelectedItems.at(0)->text(ColumnName) + "</tt> (" + cloudSelectedItems.at(0)->text(ColumnId) + ")</li>"
                     + "</ul>";
    if (RMessageBox::question(this,tr("File update"),question) != RMessageBox::Yes)
    {
        return;
    }

    try
    {
        RProgressDialog *progressDialog = new RProgressDialog(tr("File update") + ": <tt>" + localSelectedItems.at(0)->text(ColumnName) + "</tt>",tr("Cancel"),0,100,this);
        progressDialog->setJob(this->cloudClient->requestFileUpdate(localDirectory.absoluteFilePath(localSelectedItems.at(0)->text(ColumnName)),
                                                                    localSelectedItems.at(0)->text(ColumnName),
                                                                    cloudSelectedItems.at(0)->data(ColumnId,Qt::UserRole).toUuid()));
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request file upload to Cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("File upload failed"),tr("File upload to Cloud has failed."));
    }
}

void RCloudFileManagerWidget::updateSelectedCloudFilesAccessOwner()
{
    QList<QTreeWidgetItem*> selectedItems = this->cloudFilesWidget->selectedItems();
    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RFileInfo fileInfo;
        fileInfo.setId(selectedItem->data(ColumnId,Qt::UserRole).toUuid());
        fileInfo.setPath(selectedItem->text(ColumnName));

        RAccessRights accessRights;
        accessRights.setOwner(RAccessOwner::fromString(selectedItem->text(ColumnAccessOwner),':'));

        fileInfo.setAccessRights(accessRights);

        RAccessOwnerDialog cloudAccessOwnerDialog(fileInfo.getId(),fileInfo.getPath(),fileInfo.getAccessRights().getOwner(),this);
        if (cloudAccessOwnerDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestFileUpdateAccessOwner(cloudAccessOwnerDialog.getAccessOwner(),fileInfo.getId());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request file access owner update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("File access owner update failed"),tr("File access owner update in Cloud has failed."));
            }
        }
    }
}

void RCloudFileManagerWidget::updateSelectedCloudFilesAccessMode()
{
    QList<QTreeWidgetItem*> selectedItems = this->cloudFilesWidget->selectedItems();
    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RFileInfo fileInfo;
        fileInfo.setId(selectedItem->data(ColumnId,Qt::UserRole).toUuid());
        fileInfo.setPath(selectedItem->text(ColumnName));

        RAccessRights accessRights;
        accessRights.setMode(RAccessMode::fromHuman(selectedItem->text(ColumnAccessMode)));

        fileInfo.setAccessRights(accessRights);

        RAccessModeDialog cloudAccessModeDialog(fileInfo.getId(),fileInfo.getPath(),fileInfo.getAccessRights().getMode(),RAccessMode::RW,this);
        if (cloudAccessModeDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestFileUpdateAccessMode(cloudAccessModeDialog.getAccessMode(),fileInfo.getId());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request file access mode update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("File access mode update failed"),tr("File access mode update in Cloud has failed."));
            }
        }
    }
}

void RCloudFileManagerWidget::updateSelectedCloudFilesVersion()
{
    QList<QTreeWidgetItem*> selectedItems = this->cloudFilesWidget->selectedItems();
    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RFileInfo fileInfo;
        fileInfo.setId(selectedItem->data(ColumnId,Qt::UserRole).toUuid());
        fileInfo.setPath(selectedItem->text(ColumnName));
        fileInfo.setVersion(RVersion::fromString(selectedItem->text(ColumnVersion),'.'));

        RFileVersionDialog cloudFileVersionDialog(fileInfo,this);
        if (cloudFileVersionDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestFileUpdateVersion(cloudFileVersionDialog.getVersion(),fileInfo.getId());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request file version update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("File version update failed"),tr("File version update in Cloud has failed."));
            }
        }
    }
}

void RCloudFileManagerWidget::updateSelectedCloudFilesTags()
{
    QList<QTreeWidgetItem*> selectedItems = this->cloudFilesWidget->selectedItems();
    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RFileInfo fileInfo;
        fileInfo.setId(selectedItem->data(ColumnId,Qt::UserRole).toUuid());
        fileInfo.setPath(selectedItem->text(ColumnName));
        fileInfo.setTags(selectedItem->text(ColumnTags).split(',',Qt::SkipEmptyParts));

        RFileTagsDialog cloudFileTagsDialog(fileInfo,this);
        if (cloudFileTagsDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestFileUpdateTags(cloudFileTagsDialog.getTags(),fileInfo.getId());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request file tags update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("File tags update failed"),tr("File tags update in Cloud has failed."));
            }
        }
    }
}

void RCloudFileManagerWidget::removeSelectedLocalFiles()
{
    QString question = tr("Are you sure you want to remove selected local files?");
    question += "<ul>";
    QList<QTreeWidgetItem*> selectedItems = this->localFilesWidget->selectedItems();
    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        question += "<li><tt>" + selectedItem->text(ColumnName) + "</tt></li>";
    }
    question += "</ul>";
    if (RMessageBox::question(this,tr("File remove"),question) != RMessageBox::Yes)
    {
        return;
    }

    QDir localDirectory(this->localDirectoryPath);

    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        QString filePath = localDirectory.absoluteFilePath(selectedItem->text(ColumnName));

        if (QFile::remove(filePath))
        {
            RLogger::info("Local file removed \'%s\'\n",filePath.toUtf8().constData());
        }
        else
        {
            RLogger::error("Failed to remove local file \'%s\'\n",filePath.toUtf8().constData());
            RMessageBox::critical(this,tr("File remove failed"),tr("Local file remove has failed."));
        }
    }
}

void RCloudFileManagerWidget::downloadSelectedCloudFiles()
{
    QDir localDirectory(this->localDirectoryPath);

    QList<QTreeWidgetItem*> selectedItems = this->cloudFilesWidget->selectedItems();
    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        QString _fileName = selectedItem->text(ColumnName);
        QUuid _fileUuid = selectedItem->data(ColumnId,Qt::UserRole).toUuid();
        try
        {
            QString filePath = localDirectory.absoluteFilePath(QFileInfo(_fileName).fileName());
            int retVal = RMessageBox::Yes;
            if (QFileInfo::exists(filePath))
            {
                QString question = tr("Local file already exists.") + "<pre>" + filePath + "</pre>"
                                   + tr("Would you like to overwrite it?");
                retVal = RMessageBox::question(this,tr("File download"),question);
                if (retVal == RMessageBox::No)
                {
                    QFileInfo fileInfo(filePath);
                    QString path = fileInfo.path();
                    QString baseName = fileInfo.completeBaseName();

                    uint increment = 1;

                    QFileInfo baseNameFileInfo(baseName);
                    if (!baseNameFileInfo.suffix().isEmpty())
                    {
                        bool isOk = false;
                        uint possibleIncrement = baseNameFileInfo.suffix().toUInt(&isOk);
                        if (isOk)
                        {
                            increment = possibleIncrement;
                        }
                    }

                    QString suffix = fileInfo.suffix();

                    QDir pathDir(path);
                    do
                    {
                        QString fileName = baseName + "." + QString::number(increment) + "." + suffix;
                        filePath = pathDir.filePath(fileName);
                        increment++;
                    } while (QFileInfo::exists(filePath));

                    QString question = tr("Would you like to store remote file under different name?") + "<pre>" + filePath + "</pre>";
                    retVal = RMessageBox::question(this,tr("File download"),question);
                }
            }
            if (retVal == RMessageBox::Yes)
            {
                RProgressDialog *progressDialog = new RProgressDialog(tr("File download") + ": <tt>" + _fileName + "</tt>",tr("Cancel"),0,100,this);
                progressDialog->setJob(this->cloudClient->requestFileDownload(filePath,_fileUuid));
            }
        }
        catch (const RError &rError)
        {
            RLogger::error("Failed to request file download from Cloud. %s\n",rError.getMessage().toUtf8().constData());
            RMessageBox::critical(this,tr("File download failed"),tr("File download from Cloud has failed."));
        }
    }
}

void RCloudFileManagerWidget::removeSelectedCloudFiles()
{
    QString question = tr("Are you sure you want to remove selected Cloud files?");
    question += "<ul>";
    QList<QTreeWidgetItem*> selectedItems = this->cloudFilesWidget->selectedItems();
    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        question += "<li><tt>" + selectedItem->text(ColumnName) + "</tt> (" + selectedItem->text(ColumnId) + ")</li>";
    }
    question += "</ul>";
    if (RMessageBox::question(this,tr("File remove"),question) != RMessageBox::Yes)
    {
        return;
    }

    for (const QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        try
        {
            this->cloudClient->requestFileRemove(selectedItem->data(ColumnId,Qt::UserRole).toUuid());
        }
        catch (const RError &rError)
        {
            RLogger::error("Failed to request file removal from Cloud. %s\n",rError.getMessage().toUtf8().constData());
            RMessageBox::critical(this,tr("File removal failed"),tr("File removal from Cloud has failed."));
        }
    }
}

void RCloudFileManagerWidget::onLocalUploadButtonClicked()
{
    this->uploadSelectedLocalFiles();
}

void RCloudFileManagerWidget::onLocalUpdateButtonClicked()
{
    this->updateSelectedLocalFiles();
}

void RCloudFileManagerWidget::onLocalDeleteButtonClicked()
{
    this->removeSelectedLocalFiles();
}

void RCloudFileManagerWidget::onCloudDownloadButtonClicked()
{
    this->downloadSelectedCloudFiles();
}

void RCloudFileManagerWidget::onCloudAccessOwnerActionTriggered()
{
    this->updateSelectedCloudFilesAccessOwner();
}

void RCloudFileManagerWidget::onCloudAccessModeActionTriggered()
{
    this->updateSelectedCloudFilesAccessMode();
}

void RCloudFileManagerWidget::onCloudVersionActionTriggered()
{
    this->updateSelectedCloudFilesVersion();
}

void RCloudFileManagerWidget::onCloudTagsActionTriggered()
{
    this->updateSelectedCloudFilesTags();
}

void RCloudFileManagerWidget::onCloudRefreshButtonClicked()
{
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onRefreshTimeout()
{
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onRefreshTimeoutChanged(uint refreshTimeout)
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

void RCloudFileManagerWidget::onClientConfigurationChanged()
{
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onCloudDeleteButtonClicked()
{
    this->removeSelectedCloudFiles();
}

void RCloudFileManagerWidget::onLocalFileSelectionChanged()
{
    uint nLocalSelected = this->localFilesWidget->selectedItems().count();
    uint nCloudSelected = this->cloudFilesWidget->selectedItems().count();
    this->localDeleteButton->setEnabled(nLocalSelected);
    this->localUploadButton->setEnabled(nLocalSelected);
    this->localUpdateButton->setEnabled(nLocalSelected == 1 && nCloudSelected == 1);
}

void RCloudFileManagerWidget::onCloudFileSelectionChanged()
{
    uint nLocalSelected = this->localFilesWidget->selectedItems().count();
    uint nCloudSelected = this->cloudFilesWidget->selectedItems().count();
    this->cloudDeleteButton->setEnabled(nCloudSelected);
    this->cloudDownloadButton->setEnabled(nCloudSelected);
    this->cloudEditButton->setEnabled(nCloudSelected);
    this->localUpdateButton->setEnabled(nLocalSelected == 1 && nCloudSelected == 1);
}

void RCloudFileManagerWidget::onLocalDirectoryChanged(const QString &path)
{
    if (this->localDirectoryPath != path)
    {
        this->localFileSystemWatcher->removePath(this->localDirectoryPath);
        this->localDirectoryPath = path;
        if (!this->localFileSystemWatcher->addPath(this->localDirectoryPath))
        {
            RLogger::warning("Could not add path \'%s\' to file system watcher\n",this->localDirectoryPath.toUtf8().constData());
        }
    }
    RCloudFileManagerWidget::populateLocalFiles(this->localDirectoryPath,this->localFilesWidget);
}

void RCloudFileManagerWidget::onFileListAvailable(QList<RFileInfo> fileInfoList)
{
    RCloudFileManagerWidget::populateCloudFiles(fileInfoList,this->cloudFilesWidget);
}

void RCloudFileManagerWidget::onFileUploaded(RFileInfo fileInfo)
{
    RLogger::info("Cloud file uploaded \'%s\'\n",fileInfo.getPath().toUtf8().constData());
    RMessageBox::information(this,tr("File uploaded"),tr("File was uploaded successfully."));
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onFileUpdated(RFileInfo fileInfo)
{
    RLogger::info("Cloud file updated \'%s\'\n",fileInfo.getPath().toUtf8().constData());
    RMessageBox::information(this,tr("File updated"),tr("File was updated successfully."));
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onFileAccessModeUpdated(RFileInfo fileInfo)
{
    RLogger::info("Cloud file access mode updated \'%s\'\n",fileInfo.getPath().toUtf8().constData());
    RMessageBox::information(this,tr("File access mode updated"),tr("File access mode was updated successfully."));
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onFileAccessOwnerUpdated(RFileInfo fileInfo)
{
    RLogger::info("Cloud file access owner updated \'%s\'\n",fileInfo.getPath().toUtf8().constData());
    RMessageBox::information(this,tr("File access owner updated"),tr("File access owner was updated successfully."));
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onFileVersionUpdated(RFileInfo fileInfo)
{
    RLogger::info("Cloud file version updated \'%s\'\n",fileInfo.getPath().toUtf8().constData());
    RMessageBox::information(this,tr("File version updated"),tr("File version was updated successfully."));
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onFileTagsUpdated(RFileInfo fileInfo)
{
    RLogger::info("Cloud file tags updated \'%s\'\n",fileInfo.getPath().toUtf8().constData());
    RMessageBox::information(this,tr("File tags updated"),tr("File tags were updated successfully."));
    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onFileDownloaded(QString fileName)
{
    RLogger::info("Cloud file downloaded \'%s\'\n",fileName.toUtf8().constData());
    RMessageBox::information(this,tr("File downloaded"),tr("File was downloaded successfully."));
    RCloudFileManagerWidget::populateLocalFiles(this->localDirectoryPath,this->localFilesWidget);
}

void RCloudFileManagerWidget::onFileRemoved(RFileInfo fileInfo)
{
    RLogger::info("Cloud file removed \'%s\'\n",fileInfo.getPath().toUtf8().constData());
    RMessageBox::information(this,tr("File removed"),tr("File was removed successfully."));

    this->refreshCloudFiles();
}

void RCloudFileManagerWidget::onClientSubmitted()
{
    // this->progressDialog->setValue(this->progressDialog->minimum());
}

void RCloudFileManagerWidget::onClientFinished()
{
    // this->progressDialog->setValue(this->progressDialog->maximum());
}

void RCloudFileManagerWidget::onClientFailed()
{
    // this->progressDialog->setValue(this->progressDialog->maximum());
}
