#include "rgl_ai_agent_settings_widget.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>

RAiAgentSettingsWidget::RAiAgentSettingsWidget(const RAgentSettings &settings, QWidget *parent)
    : QWidget{parent}
    , settings{settings}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QComboBox *predefinedAiCombo = new QComboBox;
    for (int i=0;i<int(RAgentSettings::Type::nTypes);i++)
    {
        RAgentSettings::Type type = RAgentSettings::Type(i);
        if (type == RAgentSettings::Type::None)
        {
            continue;
        }
        predefinedAiCombo->addItem(RAgentSettings::typeToString(type),i);
        if (type == this->settings.getType())
        {
            predefinedAiCombo->setCurrentIndex(i);
        }
    }
    // for (int i=0;i<predefinedAiCombo->count();i++)
    // {
    //     if (RAgentSettings::Type(predefinedAiCombo->itemData(i).toInt()) == this->settings.getType())
    //     {
    //         predefinedAiCombo->setCurrentIndex(i);
    //     }
    // }
    mainLayout->addWidget(predefinedAiCombo);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    QLineEdit *apiKeyEdit = new QLineEdit;
    formLayout->addRow(tr("API key"),apiKeyEdit);

    QLineEdit *apiUrlEdit = new QLineEdit;
    formLayout->addRow(tr("URL"),apiUrlEdit);

    QLineEdit *modelEdit = new QLineEdit;
    formLayout->addRow(tr("Model"),modelEdit);

    auto setSettingsValues = [apiKeyEdit,apiUrlEdit,modelEdit](const RAgentSettings &agentSettings)
    {
        apiKeyEdit->setText(agentSettings.getApiKey());
        apiUrlEdit->setText(agentSettings.getApiUrl());
        modelEdit->setText(agentSettings.getModel());
    };

    setSettingsValues(this->settings);

    QObject::connect(predefinedAiCombo,&QComboBox::currentIndexChanged,[=](int index) {
        RAgentSettings::Type agentType = RAgentSettings::Type(predefinedAiCombo->itemData(index).toInt());
        setSettingsValues(RAgentSettings(agentType));
    });

    QObject::connect(apiKeyEdit,&QLineEdit::textChanged,[this](const QString &text) {
        this->settings.setApiKey(text);
        emit this->settigsChanged(this->settings);
    });

    QObject::connect(apiUrlEdit,&QLineEdit::textChanged,[this](const QString &text) {
        this->settings.setApiUrl(text);
        emit this->settigsChanged(this->settings);
    });

    QObject::connect(modelEdit,&QLineEdit::textChanged,[this](const QString &text) {
        this->settings.setModel(text);
        emit this->settigsChanged(this->settings);
    });
}
