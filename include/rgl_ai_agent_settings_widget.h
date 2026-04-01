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
        explicit RAiAgentSettingsWidget(const RAgentSettings &settings, QWidget *parent = nullptr);

    signals:

        void settigsChanged(const RAgentSettings &settings);

};

#endif // RGL_AI_AGENT_SETTINGS_WIDGET_H
