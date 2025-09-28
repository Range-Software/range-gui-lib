#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
#include <QDialogButtonBox>

#include <rbl_utils.h>

#include "rgl_first_run_dialog.h"
#include "rgl_application.h"

RFirstRunDialog::RFirstRunDialog(QWidget *parent)
    : QDialog(parent)
{
    QIcon startIcon(":/icons/application/pixmaps/range-startup.svg");
    QIcon rangeIcon(":/icons/logos/pixmaps/range-app.svg");

    this->setWindowTitle(tr("First run"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QHBoxLayout *logoLayout = new QHBoxLayout;
    mainLayout->addLayout(logoLayout);

    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(rangeIcon.pixmap(QSize(128,128)));
    logoLayout->addWidget(logoLabel);

    QString labelMsg = "<h2>" + RVendor::name() + "</h2>"
                       + "<h3>" + RVendor::description() + "</h3>"
                     + "<p>" + tr("version") + ": " + RVendor::version().toString() + "</p>"
                     + "<p><strong>" + tr("Welcome") + "!</strong></p>";

    QLabel *label = new QLabel(labelMsg);
    logoLayout->addWidget(label);

    QLabel *sendUsageInfoLabel = new QLabel(tr("You can allow the sending of usage info from your computer so that Range Software can receive bug reports and statistics that help improve our software. None of the information gathered includes personal data."));
    sendUsageInfoLabel->setWordWrap(true);
    mainLayout->addWidget(sendUsageInfoLabel);

    QCheckBox *sendUsageInfoCheck = new QCheckBox(tr("Send usage information"));
    sendUsageInfoCheck->setCheckState(RApplication::instance()->getApplicationSettings()->getSoftwareSendUsageInfo()?Qt::Checked:Qt::Unchecked);
    mainLayout->addWidget(sendUsageInfoCheck);

    QObject::connect(sendUsageInfoCheck,&QCheckBox::toggled,this,&RFirstRunDialog::onSendUsageInfoToggled);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *startButton = new QPushButton(startIcon, tr("Start"));
    buttonBox->addButton(startButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

void RFirstRunDialog::onSendUsageInfoToggled(bool checked)
{
    RApplication::instance()->getApplicationSettings()->setSoftwareSendUsageInfo(checked);
}
