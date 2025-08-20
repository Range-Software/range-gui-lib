#ifndef RGL_PROGRESS_DIALOG_H
#define RGL_PROGRESS_DIALOG_H

#include <QProgressDialog>

class RProgressDialog : public QProgressDialog
{
    Q_OBJECT

    public:

        //! Constructor.
        explicit RProgressDialog(QWidget *parent = nullptr,
                                 Qt::WindowFlags f = Qt::WindowFlags());

        //! Constructor.
        explicit RProgressDialog(const QString &labelText,
                                 const QString &cancelButtonText,
                                 int minimum,
                                 int maximum,
                                 QWidget *parent = nullptr,
                                 Qt::WindowFlags f = Qt::WindowFlags());

    public slots:

        //

};

#endif // RGL_PROGRESS_DIALOG_H
