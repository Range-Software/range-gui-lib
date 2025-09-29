#include <QFile>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QDialogButtonBox>

#include <rcl_cloud_session_manager.h>
#include <rcl_report_record.h>

#include "rgl_crash_report_dialog.h"
#include "rgl_message_box.h"

RCrashReportDialog::RCrashReportDialog(RApplicationSettings *applicationSettings,
                                       const QString &logFile,
                                       QWidget *parent)
    : QDialog{parent}
{
    this->reportSender = new RCloudReportSender(applicationSettings,this);

    QObject::connect(this->reportSender,&RCloudReportSender::reportSent,this,&RCrashReportDialog::onReportSent);
    QObject::connect(this->reportSender,&RCloudReportSender::reportFailed,this,&RCrashReportDialog::onReportFailed);

    bool hasLogFile = QFile::exists(logFile);

    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-ok.svg");

    this->setWindowTitle(tr("Crash report"));
    this->resize(900,600);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    mainLayout->addLayout(titleLayout);

    QLabel *titlePixmapLabel = new QLabel;
    titlePixmapLabel->setPixmap(QIcon::fromTheme(QIcon::ThemeIcon::DialogWarning).pixmap(64));
    titleLayout->addWidget(titlePixmapLabel);

    QLabel *titleLabel = new QLabel(tr("Previous instance has terminated unexpectedly.") + "\n" + tr("Would you like to report the crash?"));
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    titleLayout->addWidget(titleLabel);

    if (hasLogFile)
    {
        QGroupBox *logGroupBox = new QGroupBox(tr("Log file from crashed instance"));
        logGroupBox->setCheckable(false);
        logGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mainLayout->addWidget(logGroupBox);

        QVBoxLayout *logGroupLayout = new QVBoxLayout;
        logGroupBox->setLayout(logGroupLayout);

        this->logBrowser = new RLogBrowser(logFile);
        logGroupLayout->addWidget(logBrowser);
    }

    QGroupBox *userGroupBox = new QGroupBox(tr("Additional information"));
    userGroupBox->setCheckable(false);
    userGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(userGroupBox);

    QVBoxLayout *userGroupLayout = new QVBoxLayout;
    userGroupBox->setLayout(userGroupLayout);

    this->textEdit = new QPlainTextEdit;
    userGroupLayout->addWidget(this->textEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QPushButton *reportButton = new QPushButton(okIcon, tr("Send crash report"));
    buttonBox->addButton(reportButton,QDialogButtonBox::ApplyRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(reportButton,&QPushButton::clicked,this,&RCrashReportDialog::onReportClicked);
}

void RCrashReportDialog::onReportClicked()
{
    RReportRecord reportRecord;
    reportRecord.setReport(this->logBrowser->toPlainText());
    reportRecord.setComment(this->textEdit->toPlainText());
    this->reportSender->sendReport(reportRecord);
}

void RCrashReportDialog::onReportSent(QString message)
{
    RMessageBox::information(this,tr("Report sent"),message);
    this->close();
}

void RCrashReportDialog::onReportFailed(RError::Type errorType, const QString &errorMessage, const QString &message)
{
    RMessageBox::critical(this,tr("Failed to send crash report"),message,errorType,errorMessage);
}
