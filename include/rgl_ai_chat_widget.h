#ifndef RGL_AI_CHAT_WIDGET_H
#define RGL_AI_CHAT_WIDGET_H

#include <rai_agent_settings_manager.h>
#include <rgl_application_settings.h>

#include <QThread>
#include <QWidget>

class QComboBox;
class RAgent;

class RAiChatWidget : public QWidget
{
    Q_OBJECT

    QThread *agentThread = nullptr;
    RAgentSettingsManager *agentSettingsManager = nullptr;
    RApplicationSettings *appSettings = nullptr;

    void onAgentSelected(const QString &name);

    //! Ask the user for permission to send data to the selected AI agent.
    bool requestDataSharingConsent();

protected:

    RAgent *agent = nullptr;
    QComboBox *aiAgentCombo = nullptr;
    QComboBox *languageCombo = nullptr;

    virtual QString buildPrompt(const QString &language) const = 0;
    void applyCurrentAgent();

Q_SIGNALS:

    void aiAgentNameChanged(const QString &name);
    void languageNameChanged(const QString &name);

public:

    //! Constructor.
    explicit RAiChatWidget(RAgentSettingsManager *aiAgentSettingsManager,
                           RApplicationSettings *applicationSettings,
                           const QString &initialAgent = QString(),
                           const QString &initialLanguage = QString(),
                           QWidget *parent = nullptr);

    //! Destructor.
    ~RAiChatWidget() override;

};

#endif // RGL_AI_CHAT_WIDGET_H
