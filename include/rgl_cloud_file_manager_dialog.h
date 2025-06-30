#ifndef RGL_CLOUD_FILE_MANAGER_DIALOG_H
#define RGL_CLOUD_FILE_MANAGER_DIALOG_H

#include <QDialog>
#include <QWidget>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"

class RCloudFileManagerDialog : public QDialog
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RCloudFileManagerDialog(RCloudConnectionHandler *connectionHandler,
                                         RApplicationSettings *applicationSettings,
                                         bool allowDirectorySelector,
                                         QWidget *parent = nullptr);

};

#endif // RGL_CLOUD_FILE_MANAGER_DIALOG_H
