#ifndef RGL_CLOUD_AI_QUERY_DIALOG_H
#define RGL_CLOUD_AI_QUERY_DIALOG_H

#include <QDialog>
#include <QWidget>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"

class RCloudAiQueryDialog : public QDialog
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RCloudAiQueryDialog(RCloudConnectionHandler *connectionHandler,
                                   RApplicationSettings *applicationSettings,
                                   QWidget *parent = nullptr);

};

#endif // RGL_CLOUD_AI_QUERY_DIALOG_H
