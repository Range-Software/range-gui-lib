#ifndef RGL_SOFTWARE_MANAGER_WIDGET_H
#define RGL_SOFTWARE_MANAGER_WIDGET_H

#include <QPushButton>
#include <QCheckBox>

#include <rcl_file_info.h>
#include <rcl_http_proxy_settings.h>
#include <rcl_software_manager.h>

#include "rgl_application_settings.h"
#include "rgl_tree_widget.h"

class RSoftwareManagerWidget : public QWidget
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

        RTreeWidget *cloudFilesWidget;
        QCheckBox *allSoftwareCheckBox;
        QPushButton *installButton;

        RSoftwareManager *softwareManager;

        bool showProgress;

    public:

        //! Constructor.
        explicit RSoftwareManagerWidget(const RApplicationSettings *applicationSettings, QWidget *parent = nullptr);

    protected:

        //! Trigger request to list files.
        void refreshCloudFiles();

        static QString getColumnName(ColumnType columnType);

        static void populateFiles(const QList<RFileInfo> &fileInfoList, QTreeWidget *treeWidget);

        static void updateFileListItem(QTreeWidgetItem *listItem, const RFileInfo &fileInfo);

        static QStringList getExpectedTags(bool allSoftwareChecked);

        static QString getAllSoftwareCheckBoxLabel(bool allSoftwareChecked);

    private slots:

        void onItemSelectionChanged();

        void onAllSoftwareBoxToggled(bool checked);

        void onInstallClicked();

    protected slots:

        //! Software is available.
        void onSoftwareAvailable(QList<RFileInfo> fileInfoList);

        //! Software is downloaded.
        void onSoftwareDownloaded(QString fileName);

        //! Proxy settings have changed.
        void onProxySettingsChanged(RHttpProxySettings proxySettings);

};

#endif // RGL_SOFTWARE_MANAGER_WIDGET_H
