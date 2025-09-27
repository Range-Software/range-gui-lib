#include "rgl_progress_dialog.h"

RProgressDialog::RProgressDialog(QWidget *parent, Qt::WindowFlags f)
    : QProgressDialog{parent,f}
    , job{nullptr}
{
    this->setMinimumDuration(1000);
}

RProgressDialog::RProgressDialog(const QString &labelText,
                                 const QString &cancelButtonText,
                                 int minimum,
                                 int maximum,
                                 QWidget *parent,
                                 Qt::WindowFlags f)
    : QProgressDialog{labelText,cancelButtonText,minimum,maximum,parent,f}
    , job{nullptr}
{
    this->setMinimumDuration(1000);
}

const RJob *RProgressDialog::getJob() const
{
    return this->job;
}

void RProgressDialog::setJob(const RJob *job)
{
    this->job = job;

    if (this->job)
    {
        QObject::connect(this,&RProgressDialog::canceled,this->job,&RJob::cancel);
        QObject::connect(this->job, &RJob::progressChanged, this, &RProgressDialog::setProgress);
        QObject::connect(this->job, &RJob::finished, this, &RProgressDialog::deleteLater);
        QObject::connect(this->job, &RJob::failed, this, &RProgressDialog::deleteLater);
    }
}

void RProgressDialog::setProgress(qint64 step, qint64 nSteps)
{
    double fraction = (double(step)/double(nSteps+1))*100.0;
    this->setValue(qRound(fraction));
}
