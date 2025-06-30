#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "rgl_cloud_file_manager_dialog.h"
#include "rgl_cloud_file_manager_widget.h"

RCloudFileManagerDialog::RCloudFileManagerDialog(RCloudConnectionHandler *connectionHandler,
                                                 RApplicationSettings *applicationSettings,
                                                 bool allowDirectorySelector,
                                                 QWidget *parent)
    : QDialog{parent}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Cloud file manager"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RCloudFileManagerWidget *cloudFimeManagerWidget = new RCloudFileManagerWidget(connectionHandler,applicationSettings,allowDirectorySelector);
    mainLayout->addWidget(cloudFimeManagerWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);
}
