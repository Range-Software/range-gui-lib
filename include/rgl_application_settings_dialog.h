#ifndef RGL_APPLICATION_SETTINGS_DIALOG_H
#define RGL_APPLICATION_SETTINGS_DIALOG_H

#include <QDialog>

#include "rgl_application_settings.h"

class RApplicationSettingsDialog : public QDialog
{
    Q_OBJECT

    public:

        //! Constructor.
        explicit RApplicationSettingsDialog(RApplicationSettings *mainSettings, QWidget *parent = nullptr);

};

#endif // RGL_APPLICATION_SETTINGS_DIALOG_H
