#include "rgl_progress_dialog.h"

RProgressDialog::RProgressDialog(QWidget *parent, Qt::WindowFlags f)
    : QProgressDialog{parent,f}
{

}

RProgressDialog::RProgressDialog(const QString &labelText,
                                 const QString &cancelButtonText,
                                 int minimum,
                                 int maximum,
                                 QWidget *parent,
                                 Qt::WindowFlags f)
    : QProgressDialog{labelText,cancelButtonText,minimum,maximum,parent,f}
{

}
