#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QFile>
#include <QPushButton>
#include <QDialogButtonBox>

#include "rgl_log_browser_dialog.h"

RLogBrowserDialog::RLogBrowserDialog(const QString &logFileName, QWidget *parent) :
    QDialog(parent)
{
    QIcon clearIcon(":/icons/action/pixmaps/range-clear.svg");
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Log file viewer"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QLabel *label = new QLabel("Log: <i>" + logFileName + "</i>");
    mainLayout->addWidget(label);

    this->logBrowser = new RLogBrowser(logFileName,this);
    mainLayout->addWidget(this->logBrowser);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *clearButton = new QPushButton(clearIcon, tr("Clear"));
    buttonBox->addButton(clearButton,QDialogButtonBox::ResetRole);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(clearButton,&QPushButton::clicked,this,&RLogBrowserDialog::onClearButtonClicked);
}

void RLogBrowserDialog::onClearButtonClicked()
{
    this->logBrowser->clearLog();
}
