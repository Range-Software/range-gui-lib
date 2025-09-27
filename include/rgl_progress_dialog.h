#ifndef RGL_PROGRESS_DIALOG_H
#define RGL_PROGRESS_DIALOG_H

#include <QProgressDialog>

#include <rbl_job.h>

class RProgressDialog : public QProgressDialog
{
    Q_OBJECT

    protected:

        //! Task object
        const RJob *job;

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

        //! Get tool task.
        const RJob *getJob() const;

        //! Set tool task.
        void setJob(const RJob *job);

    public slots:

        //! Set progress value based on curent step and number of steps.
        void setProgress(qint64 step, qint64 nSteps);

};

#endif // RGL_PROGRESS_DIALOG_H
