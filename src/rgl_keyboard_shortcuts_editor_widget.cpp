#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

#include "rgl_keyboard_shortcuts_editor_widget.h"
#include "rgl_message_box.h"

RKeyboardShortcutsEditorWidget::RKeyboardShortcutsEditorWidget(const RActionList *actionList, QWidget *parent) :
    QWidget{parent},
    actionList{actionList}
{
    QVBoxLayout *topLayout = new QVBoxLayout;
    this->setLayout(topLayout);

    QGroupBox *keyboardShortcutsBox = new QGroupBox(tr("Keayboard Shortcuts"));
    topLayout->addWidget(keyboardShortcutsBox);

    QGridLayout *keyboardShortcutsLayout = new QGridLayout;
    keyboardShortcutsBox->setLayout(keyboardShortcutsLayout);

    this->populateTree();

    keyboardShortcutsLayout->addWidget(this->tree,0,0,1,2);

    QObject::connect(this->tree,
                     &QTreeWidget::itemSelectionChanged,
                     this,
                     &RKeyboardShortcutsEditorWidget::onSelectionChanged);

    QGroupBox *shortcutBox = new QGroupBox(tr("Shortcut"));
    topLayout->addWidget(shortcutBox);

    QHBoxLayout *shortcutLayout = new QHBoxLayout;
    shortcutBox->setLayout(shortcutLayout);

    QLabel *shortcutLabel = new QLabel(tr("Target") + ":");
    shortcutLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    shortcutLayout->addWidget(shortcutLabel);

    this->lineEdit = new RKeyboardShortcutLineEdit;
    this->lineEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    this->lineEdit->setDisabled(true);
    shortcutLayout->addWidget(this->lineEdit);

    QObject::connect(this->lineEdit,
                     &RKeyboardShortcutLineEdit::shortcutChanged,
                     this,
                     &RKeyboardShortcutsEditorWidget::onShortcutChanged);
}

void RKeyboardShortcutsEditorWidget::resetToDefault()
{
    for (int i=0;i<this->tree->topLevelItemCount();i++)
    {
        for (int j=0;j<this->tree->topLevelItem(i)->childCount();j++)
        {
            uint position = this->tree->topLevelItem(i)->child(j)->data(0,Qt::UserRole).toUInt();
            const QString &shortcut = this->actionList->getAction(position)->getDefaultShortcut();
            if (this->tree->topLevelItem(i)->child(j)->text(1) != shortcut)
            {
                this->tree->blockSignals(true);
                this->tree->topLevelItem(i)->child(j)->setText(1,shortcut);
                this->tree->blockSignals(false);
                emit this->shortcutChanged(position,shortcut);
            }
        }
    }
}

void RKeyboardShortcutsEditorWidget::populateTree()
{
    this->tree = new QTreeWidget;
    this->tree->setSelectionMode(QAbstractItemView::SingleSelection);
    this->tree->setSortingEnabled(true);
    this->tree->setColumnCount(2);
    this->tree->sortByColumn(0,Qt::AscendingOrder);

    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,QString("Action"));
    headerItem->setText(1,QString("Shortcut"));
    this->tree->setHeaderItem(headerItem);
    this->tree->setRootIsDecorated(true);

    QStringList groupNames = this->actionList->findGroupNames();

    for (const QString &groupName : std::as_const(groupNames))
    {
        QTreeWidgetItem *itemGroup = new QTreeWidgetItem(this->tree);

        itemGroup->setText(0,groupName);
        itemGroup->setData(0,Qt::UserRole,0);
        itemGroup->setForeground(0,this->palette().brush(QPalette::Active,QPalette::Text));
        QFont font(itemGroup->font(0));
        font.setBold(true);
        itemGroup->setFont(0,font);

        for (uint i=0;i<this->actionList->getNActions();i++)
        {
            const RAction *action = this->actionList->getAction(i);
            if (action == nullptr || action->isSeparator() || action->getGroupName() != groupName)
            {
                continue;
            }

            QTreeWidgetItem *item = new QTreeWidgetItem(itemGroup);

            item->setText(0,action->text());
            item->setData(0,Qt::UserRole,i);
            item->setText(1,action->shortcut().toString());
        }
    }

    this->tree->expandAll();

    this->tree->resizeColumnToContents(0);
    this->tree->resizeColumnToContents(1);

    this->tree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

QList<QTreeWidgetItem *> RKeyboardShortcutsEditorWidget::findItems(const QString &shortcut)
{
    QList<QTreeWidgetItem*> items;
    for (int i=0;i<this->tree->topLevelItemCount();i++)
    {
        for (int j=0;j<this->tree->topLevelItem(i)->childCount();j++)
        {
            QTreeWidgetItem *item = this->tree->topLevelItem(i)->child(j);
            if (item->text(1) == shortcut)
            {
                items.push_back(item);
            }
        }
    }
    return items;
}

void RKeyboardShortcutsEditorWidget::onSelectionChanged()
{
    this->lineEdit->setDisabled(true);
    this->lineEdit->clear();

    QList<QTreeWidgetItem*> selectedItems = this->tree->selectedItems();

    for (const QTreeWidgetItem *item : std::as_const(selectedItems))
    {
        if (item->data(0,Qt::UserRole).toUInt() > 0)
        {
            this->lineEdit->setText(item->text(1));
            this->lineEdit->setEnabled(true);
            return;
        }
    }
}

void RKeyboardShortcutsEditorWidget::onResetAllClicked()
{
    this->resetToDefault();
}

void RKeyboardShortcutsEditorWidget::onShortcutChanged(const QString &shortcut)
{
    QList<QTreeWidgetItem *> selectedItems = this->tree->selectedItems();

    if (!shortcut.isEmpty())
    {
        QList<QTreeWidgetItem*> previousItems = this->findItems(shortcut);

        if (previousItems.size() > 0)
        {
            QString previousActions;
            for (int j=0;j<previousItems.size();j++)
            {
                previousActions += " <b>" + previousItems[j]->text(0) + "</b>";
                if (j<previousItems.size()-1)
                {
                    previousActions += ",";
                }
            }

            QString question = tr("Shortcut")
                             + " <b>" + shortcut + "</b> "
                             + tr("is already assigned to action")
                             + previousActions
                             + QString(".<br/>")
                             + tr("Are you sure you want to reassign this shortcut?");
            if (RMessageBox::question(this, tr("Assign shortcut?"), question) != RMessageBox::Yes)
            {
                return;
            }
        }

        for (int i=0;i<previousItems.size();i++)
        {
            previousItems[i]->setText(1,QString());
            emit this->shortcutChanged(previousItems[i]->data(0,Qt::UserRole).toUInt(),QString());
        }
    }

    for (QTreeWidgetItem *item : std::as_const(selectedItems))
    {
        this->lineEdit->setText(shortcut);
        item->setText(1,shortcut);

        emit this->shortcutChanged(item->data(0,Qt::UserRole).toUInt(),shortcut);
    }
}
