#ifndef RGL_SOFTWARE_MANAGER_DIALOG_H
#define RGL_SOFTWARE_MANAGER_DIALOG_H

#include <QDialog>

#include "rgl_application_settings.h"

class RSoftwareManagerDialog : public QDialog
{
    Q_OBJECT

    public:

        //! Constructor.
        explicit RSoftwareManagerDialog(const RApplicationSettings *applicationSettings, QWidget *parent = nullptr);

};

#endif // RGL_SOFTWARE_MANAGER_DIALOG_H
