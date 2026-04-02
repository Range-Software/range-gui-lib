#ifndef RGL_AI_AGENT_SETTINGS_LIST_WIDGET_H
#define RGL_AI_AGENT_SETTINGS_LIST_WIDGET_H

#include <rai_agent_settings_manager.h>

#include <QWidget>
#include <QStringList>

class RAiAgentSettingsListWidget : public QWidget
{
    Q_OBJECT

    public:

        //! Constructor.
        explicit RAiAgentSettingsListWidget(RAgentSettingsManager *aiSettingsManager, QWidget *parent = nullptr);

    signals:

        //! List selection has changed. If nothing is selected an empty list will be provided.
        void selectionChanged(QStringList selectedNames);
};

#endif // RGL_AI_AGENT_SETTINGS_LIST_WIDGET_H
