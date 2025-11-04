#ifndef RGL_CLOUD_FILE_MANAGER_WIDGET_H
#define RGL_CLOUD_FILE_MANAGER_WIDGET_H

#include <rcl_file_info.h>
#include <rcl_cloud_client.h>

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QWidget>
#include <QPushButton>
#include <QProgressDialog>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"
#include "rgl_tree_widget.h"

class RCloudFileManagerWidget : public QWidget
{
    Q_OBJECT

    protected:

        enum ColumnType
        {
            ColumnId = 0,
            ColumnName,
            ColumnSize,
            ColumnCreationDateTime,
            ColumnUpdateDateTime,
            ColumnAccessMode,
            ColumnAccessOwner,
            ColumnVersion,
            ColumnTags,
            ColumnMd5Checksum,
            NunberOfColumns
        };

        RApplicationSettings *applicationSettings;

        RCloudClient *cloudClient;

        RTreeWidget *localFilesWidget;
        RTreeWidget *cloudFilesWidget;

        QPushButton *localUploadButton;
        QPushButton *localReplaceButton;
        QPushButton *localUpdateButton;
        QPushButton *localDeleteButton;

        QPushButton *cloudRefreshButton;
        QPushButton *cloudDeleteButton;
        QPushButton *cloudDownloadButton;
        QPushButton *cloudEditButton;

        QProgressDialog *progressDialog;

        QFileSystemWatcher *localFileSystemWatcher;

        bool allowDirectorySelector;
        QString localDirectoryPath;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        explicit RCloudFileManagerWidget(RCloudConnectionHandler *connectionHandler,
                                         RApplicationSettings *applicationSettings,
                                         bool allowDirectorySelector,
                                         QWidget *parent = nullptr);

    protected:

        static void populateLocalFiles(const QString &path, QTreeWidget *treeWidget);

        static void populateCloudFiles(const QList<RFileInfo> &fileInfoList, QTreeWidget *treeWidget);

        static void updateFileListItem(QTreeWidgetItem *listItem, const QFileInfo &fileInfo);

        static void updateFileListItem(QTreeWidgetItem *listItem, const RFileInfo &fileInfo);

        static QString getColumnName(ColumnType columnType);

        void refreshCloudFiles();

        void uploadSelectedLocalFiles();

        void replaceSelectedLocalFiles();

        void updateSelectedLocalFiles();

        void updateSelectedCloudFilesAccessOwner();

        void updateSelectedCloudFilesAccessMode();

        void updateSelectedCloudFilesVersion();

        void updateSelectedCloudFilesTags();

        void removeSelectedLocalFiles();

        void downloadSelectedCloudFiles();

        void removeSelectedCloudFiles();

    protected slots:

        //! Upload file button clicked.
        void onLocalUploadButtonClicked();

        //! Replace file button clicked.
        void onLocalReplaceButtonClicked();

        //! Update file button clicked.
        void onLocalUpdateButtonClicked();

        //! Delete local file button clicked.
        void onLocalDeleteButtonClicked();

        //! Download file button clicked.
        void onCloudDownloadButtonClicked();

        //! Cloud file access owner triggered.
        void onCloudAccessOwnerActionTriggered();

        //! Cloud file access mode triggered.
        void onCloudAccessModeActionTriggered();

        //! Cloud file version triggered.
        void onCloudVersionActionTriggered();

        //! Cloud file tags triggered.
        void onCloudTagsActionTriggered();

        //! Refresh file button clicked.
        void onCloudRefreshButtonClicked();

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

        //! Cloud client configuratin has changed.
        void onClientConfigurationChanged();

        //! Delete cloud file button clicked.
        void onCloudDeleteButtonClicked();

        //! Local file selected.
        void onLocalFileSelectionChanged();

        //! Cloud file selected.
        void onCloudFileSelectionChanged();

        //! Local directory has changed.
        void onLocalDirectoryChanged(const QString &path);

        //! Could file list is available.
        void onFileListAvailable(QList<RFileInfo> fileInfoList);

        //! File was uploaded.
        void onFileUploaded(RFileInfo fileInfo);

        //! File was replaced.
        void onFileReplaced(std::tuple<RFileInfo,QList<RFileInfo>> fileInfoList);

        //! File was updated.
        void onFileUpdated(RFileInfo fileInfo);

        //! File access mode was updated.
        void onFileAccessModeUpdated(RFileInfo fileInfo);

        //! File access owner was updated.
        void onFileAccessOwnerUpdated(RFileInfo fileInfo);

        //! File version was updated.
        void onFileVersionUpdated(RFileInfo fileInfo);

        //! File tags were updated.
        void onFileTagsUpdated(RFileInfo fileInfo);

        //! File was downloaded.
        void onFileDownloaded(QString fileName);

        //! File was removed.
        void onFileRemoved(RFileInfo fileInfo);

        //! Client has submitted request.
        void onClientSubmitted();

        //! Client has finished.
        void onClientFinished();

        //! Client has failed.
        void onClientFailed();

    signals:

        //! Progress value has changed.
        void progressValueChanged(int progress);

};

#endif // RGL_CLOUD_FILE_MANAGER_WIDGET_H
