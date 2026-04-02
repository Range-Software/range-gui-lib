#include "rgl_ai_agent_settings_manager_widget.h"
#include "rgl_ai_agent_settings_list_widget.h"
#include "rgl_ai_agent_settings_widget.h"

#include <QVBoxLayout>
#include <QSplitter>
#include <QListWidgetItem>

RAiAgentSettingsManagerWidget::RAiAgentSettingsManagerWidget(RAgentSettingsManager *aiAgentSettingsManager, QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    RAiAgentSettingsListWidget *listWidget = new RAiAgentSettingsListWidget(aiAgentSettingsManager);
    splitter->addWidget(listWidget);

    RAiAgentSettingsWidget *settingsWidget = new RAiAgentSettingsWidget(aiAgentSettingsManager->findSettings(this->selectedName));
    settingsWidget->setDisabled(this->selectedName.isEmpty());
    splitter->addWidget(settingsWidget);

    QObject::connect(listWidget,&RAiAgentSettingsListWidget::selectionChanged,[this,aiAgentSettingsManager,settingsWidget](QStringList selectedNames) {
        settingsWidget->setDisabled(selectedNames.empty());
        if (!selectedNames.empty())
        {
            this->selectedName = selectedNames.at(0);
            settingsWidget->setSettings(aiAgentSettingsManager->findSettings(this->selectedName));
        }
    });

    QObject::connect(settingsWidget,&RAiAgentSettingsWidget::settigsChanged,[this,aiAgentSettingsManager,settingsWidget](const RAgentSettings &agentSettings) {
        if (!selectedName.isEmpty())
        {
            aiAgentSettingsManager->insertSettings(this->selectedName,agentSettings);
        }
    });
}
