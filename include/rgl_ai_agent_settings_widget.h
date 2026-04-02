#ifndef RGL_AI_AGENT_SETTINGS_WIDGET_H
#define RGL_AI_AGENT_SETTINGS_WIDGET_H

#include <rai_agent_settings.h>

#include <QWidget>

class RAiAgentSettingsWidget : public QWidget
{

    Q_OBJECT

    protected:

        RAgentSettings settings;

    public:

        //! Constructor.
        explicit RAiAgentSettingsWidget(const RAgentSettings &agentSettings, QWidget *parent = nullptr);

    public slots:

        //! Set new agentSettings.
        void setSettings(const RAgentSettings &agentSettings);

    signals:

        void settigsChanged(const RAgentSettings &agentSettings);
        void settigsAvailable(const RAgentSettings &agentSettings);

};

#endif // RGL_AI_AGENT_SETTINGS_WIDGET_H
