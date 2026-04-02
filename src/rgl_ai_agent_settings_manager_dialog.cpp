#include "rgl_ai_agent_settings_manager_dialog.h"
#include "rgl_ai_agent_settings_manager_widget.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

RAiAgentSettingsManagerDialog::RAiAgentSettingsManagerDialog(RAgentSettingsManager *aiAgentSettingsManager,
                                                             QWidget *parent)
    : QDialog{parent}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("AI agents settings"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RAiAgentSettingsManagerWidget *aiAgentSettingsManagerWidget = new RAiAgentSettingsManagerWidget(aiAgentSettingsManager);
    mainLayout->addWidget(aiAgentSettingsManagerWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}
