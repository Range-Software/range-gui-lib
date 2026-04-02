#include "rgl_ai_agent_settings_widget.h"
#include <rai_agent_settings.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

RAiAgentSettingsWidget::RAiAgentSettingsWidget(const RAgentSettings &agentSettings, QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    QHBoxLayout *typeLayout = new QHBoxLayout;
    formLayout->addRow(tr("API type"),typeLayout);

    QComboBox *typeCombo = new QComboBox;
    for (int i=0;i<int(RAgentSettings::Type::nTypes);i++)
    {
        typeCombo->addItem(RAgentSettings::typeToString(RAgentSettings::Type(i)),i);
    }
    typeLayout->addWidget(typeCombo);

    QPushButton *resetButton = new QPushButton(tr("Reset"));
    typeLayout->addWidget(resetButton);

    QLineEdit *apiKeyEdit = new QLineEdit;
    formLayout->addRow(tr("API key"),apiKeyEdit);

    QLineEdit *apiUrlEdit = new QLineEdit;
    formLayout->addRow(tr("URL"),apiUrlEdit);

    QLineEdit *modelEdit = new QLineEdit;
    formLayout->addRow(tr("Model"),modelEdit);

    auto setSettingsValues = [this,typeCombo,apiKeyEdit,apiUrlEdit,modelEdit](const RAgentSettings &agentSettings)
    {
        this->settings = agentSettings;

        for (int i=0;i<int(RAgentSettings::Type::nTypes);i++)
        {
            if (RAgentSettings::Type(i) == agentSettings.getType())
            {
                typeCombo->setCurrentIndex(i);
            }
        }
        apiKeyEdit->setText(agentSettings.getApiKey());
        apiUrlEdit->setText(agentSettings.getApiUrl());
        modelEdit->setText(agentSettings.getModel());

        apiKeyEdit->setDisabled(agentSettings.getType() == RAgentSettings::Type::None);
        apiUrlEdit->setDisabled(agentSettings.getType() == RAgentSettings::Type::None);
        modelEdit->setDisabled(agentSettings.getType() == RAgentSettings::Type::None);
    };

    setSettingsValues(this->settings);

    QObject::connect(this,&RAiAgentSettingsWidget::settigsAvailable,setSettingsValues);

    QObject::connect(resetButton,&QPushButton::clicked,[setSettingsValues,typeCombo]() {
        RAgentSettings::Type agentType = RAgentSettings::Type(typeCombo->currentData().toInt());
        setSettingsValues(RAgentSettings(agentType));
    });

    QObject::connect(typeCombo,&QComboBox::currentIndexChanged,[this,typeCombo](int index) {
        this->settings.setType(RAgentSettings::Type(typeCombo->itemData(index).toInt()));
        emit this->settigsChanged(this->settings);
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

void RAiAgentSettingsWidget::setSettings(const RAgentSettings &agentSettings)
{
    emit this->settigsAvailable(agentSettings);
}
