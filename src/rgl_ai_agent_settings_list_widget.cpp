#include "rgl_ai_agent_settings_list_widget.h"
#include "rgl_message_box.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>

RAiAgentSettingsListWidget::RAiAgentSettingsListWidget(RAgentSettingsManager *aiSettingsManager, QWidget *parent)
    : QWidget{parent}
{
    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");
    QIcon removeIcon(":/icons/action/pixmaps/range-remove.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QListWidget *nameList = new QListWidget;
    nameList->setSelectionMode(QAbstractItemView::SingleSelection);

    auto addItem = [nameList](const QString &name) {
        bool signalsBlocked = nameList->signalsBlocked();
        nameList->blockSignals(true);
        QListWidgetItem *item = new QListWidgetItem(nameList);
        item->setText(name);
        item->setData(Qt::UserRole,name);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        nameList->blockSignals(signalsBlocked);
        emit nameList->itemChanged(item);
    };

    for (const QString &name : aiSettingsManager->getNames())
    {
        addItem(name);
    }
    mainLayout->addWidget(nameList);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    QPushButton *addButton = new QPushButton(addIcon, tr("Add"));
    buttonsLayout->addWidget(addButton);

    QPushButton *removeButton = new QPushButton(removeIcon, tr("Remove"));
    removeButton->setDisabled(true);
    buttonsLayout->addWidget(removeButton);

    QObject::connect(aiSettingsManager,&RAgentSettingsManager::settingsAdded,addItem);

    QObject::connect(aiSettingsManager,&RAgentSettingsManager::settingsRemoved,[nameList](const QString &name) {
        for (auto item : nameList->findItems(name,Qt::MatchExactly))
        {
            nameList->takeItem(nameList->row(item));
        }
    });

    QObject::connect(aiSettingsManager,&RAgentSettingsManager::settingsRenamed,[nameList](const QString &oldName, const QString &newName) {
        for (auto item : nameList->findItems(oldName,Qt::MatchExactly))
        {
            item->setText(newName);
            item->setData(Qt::UserRole,item->text());
        }
    });

    QObject::connect(nameList,&QListWidget::itemSelectionChanged,[this,nameList,removeButton]() {
        QList<QListWidgetItem*> selectedItems = nameList->selectedItems();
        removeButton->setEnabled(!selectedItems.empty());
        QStringList selectedNames;
        for (QListWidgetItem *item : std::as_const(selectedItems))
        {
            selectedNames.append(item->text());
        }
        emit this->selectionChanged(selectedNames);
    });

    QObject::connect(nameList,&QListWidget::itemChanged,[aiSettingsManager](QListWidgetItem *item) {
        QString oldName = item->data(Qt::UserRole).toString();
        QString newName = item->text();
        if (oldName != newName)
        {
            if (aiSettingsManager->containsName(newName))
            {
                item->setText(oldName);
                RMessageBox::warning(item->listWidget(),tr("Invalid name"),tr("Suggested name already exists in the list."));
            }
            else
            {
                item->setData(Qt::UserRole,item->text());
                aiSettingsManager->renameSettings(oldName,newName);
            }
        }
    });

    QObject::connect(addButton,&QPushButton::clicked,[aiSettingsManager]() {
        QString newNameBase = tr("New AI agent");
        QString newName = newNameBase;
        uint increment = 0;
        QStringList nameList = aiSettingsManager->getNames();
        while (nameList.contains(newName))
        {
            newName = QString("%1 - %2").arg(newNameBase, QString::number(++increment));
        }
        aiSettingsManager->insertSettings(newName,RAgentSettings());
    });

    QObject::connect(removeButton,&QPushButton::clicked,[aiSettingsManager,nameList]() {
        for (QListWidgetItem *item : nameList->selectedItems())
        {
            aiSettingsManager->removeSettings(item->text());
        }
    });
}
