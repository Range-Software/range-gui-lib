#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "rgl_cloud_session_widget.h"
#include "rgl_cloud_session_dialog.h"

RCloudSessionDialog::RCloudSessionDialog(const RCloudSessionInfo &sessionInfo,
                                         RCloudConnectionHandler *connectionHandler,
                                         RApplicationSettings *applicationSettings,
                                         QWidget *parent)
    : QDialog{parent}
    , sessionInfo{sessionInfo}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-ok.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RCloudSessionWidget *cloudSessionWidget = new RCloudSessionWidget(this->sessionInfo,connectionHandler,applicationSettings);
    mainLayout->addWidget(cloudSessionWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(this->okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(cloudSessionWidget,&RCloudSessionWidget::sessionInfoChanged,this,&RCloudSessionDialog::onSessionInfoChanged);
}

const RCloudSessionInfo &RCloudSessionDialog::getSessionInfo() const
{
    return this->sessionInfo;
}

void RCloudSessionDialog::onSessionInfoChanged(const RCloudSessionInfo &sessionInfo)
{
    this->sessionInfo = sessionInfo;
    this->okButton->setEnabled(true);
}
