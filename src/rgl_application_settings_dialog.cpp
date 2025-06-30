#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "rgl_application_settings_dialog.h"
#include "rgl_application_settings_widget.h"

RApplicationSettingsDialog::RApplicationSettingsDialog(RApplicationSettings *mainSettings, QWidget *parent)
    : QDialog(parent)
{
    QIcon defaultIcon(":/icons/action/pixmaps/range-undo.svg");
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Application settings"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RApplicationSettingsWidget *applicationSettingsWidget = new RApplicationSettingsWidget(mainSettings);
    mainLayout->addWidget(applicationSettingsWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *defaultButton = new QPushButton(defaultIcon, tr("Default"));
    buttonBox->addButton(defaultButton,QDialogButtonBox::ResetRole);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);

    QObject::connect(defaultButton,&QPushButton::clicked,applicationSettingsWidget,&RApplicationSettingsWidget::setDefaultValues);
}
