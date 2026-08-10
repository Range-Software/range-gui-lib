#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "rgl_cloud_ai_query_dialog.h"
#include "rgl_cloud_ai_query_widget.h"

RCloudAiQueryDialog::RCloudAiQueryDialog(RCloudConnectionHandler *connectionHandler,
                                     RApplicationSettings *applicationSettings,
                                     QWidget *parent)
    : QDialog{parent}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Cloud AI query"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RCloudAiQueryWidget *clouddQueryWidget = new RCloudAiQueryWidget(connectionHandler,applicationSettings);
    mainLayout->addWidget(clouddQueryWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);
}
