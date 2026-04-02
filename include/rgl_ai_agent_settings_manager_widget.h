#ifndef RGL_AI_AGENT_SETTINGS_MANAGER_WIDGET_H
#define RGL_AI_AGENT_SETTINGS_MANAGER_WIDGET_H

#include <rai_agent_settings_manager.h>

#include <QWidget>

class RAiAgentSettingsManagerWidget : public QWidget
{

    Q_OBJECT

    protected:

        QString selectedName;

    public:

        //! Constructor.
        explicit RAiAgentSettingsManagerWidget(RAgentSettingsManager *aiAgentSettingsManager, QWidget *parent = nullptr);

};

#endif // RGL_AI_AGENT_SETTINGS_MANAGER_WIDGET_H
