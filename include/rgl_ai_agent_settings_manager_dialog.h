#ifndef RGL_AI_AGENT_SETTINGS_MANAGER_DIALOG_H
#define RGL_AI_AGENT_SETTINGS_MANAGER_DIALOG_H

#include <rai_agent_settings_manager.h>

#include <QDialog>
#include <QWidget>

class RAiAgentSettingsManagerDialog : public QDialog
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RAiAgentSettingsManagerDialog(RAgentSettingsManager *aiAgentSettingsManager,
                                               QWidget *parent = nullptr);

};

#endif // RGL_AI_AGENT_SETTINGS_MANAGER_DIALOG_H
