#ifndef RGL_FIRST_RUN_DIALOG_H
#define RGL_FIRST_RUN_DIALOG_H

#include <QDialog>

class RFirstRunDialog : public QDialog
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RFirstRunDialog(QWidget *parent = nullptr);

    private slots:

        //! On Send usage info toggled.
        void onSendUsageInfoToggled(bool checked);

};

#endif // RGL_FIRST_RUN_DIALOG_H
