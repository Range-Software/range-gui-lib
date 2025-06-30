#ifndef RGL_CLOUD_SESSION_DIALOG_H
#define RGL_CLOUD_SESSION_DIALOG_H

#include <QDialog>
#include <QPushButton>

#include <rcl_cloud_session_info.h>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"

class RCloudSessionDialog : public QDialog
{
    Q_OBJECT

    protected:

        RCloudSessionInfo sessionInfo;

        QPushButton *okButton;

    public:

        //! Constructor.
        explicit RCloudSessionDialog(const RCloudSessionInfo &sessionInfo,
                                     RCloudConnectionHandler *connectionHandler,
                                     RApplicationSettings *applicationSettings,
                                     QWidget *parent = nullptr);

        //! Return const reference to session info.
        const RCloudSessionInfo &getSessionInfo() const;

    protected slots:

        //! Session info has changed.
        void onSessionInfoChanged(const RCloudSessionInfo &sessionInfo);

};

#endif // RGL_CLOUD_SESSION_DIALOG_H
