#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "rgl_software_manager_dialog.h"
#include "rgl_software_manager_widget.h"

RSoftwareManagerDialog::RSoftwareManagerDialog(const RApplicationSettings *applicationSettings, QWidget *parent)
    : QDialog{parent}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Software manager"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RSoftwareManagerWidget *softwareManagerWidget = new RSoftwareManagerWidget(applicationSettings);
    mainLayout->addWidget(softwareManagerWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);

    this->resize(1024,512);
}
