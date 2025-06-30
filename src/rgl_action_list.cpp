#include <QKeySequence>

#include "rgl_action_list.h"

RActionList::RActionList(const QList<RAction::Definition> &definitions, QObject *parent)
    : QObject{parent}
{
    this->actions.resize(definitions.size(),nullptr);

    for (int i=0;i<definitions.size();i++)
    {
        this->actions[i] = new RAction(definitions.at(i),this);
    }

    this->setDisabled(false);
}

uint RActionList::getNActions() const
{
    return this->actions.size();
}

const RAction *RActionList::getAction(uint position) const
{
    return this->actions[position];
}

RAction *RActionList::getAction(uint position)
{
    return this->actions[position];
}

RAction *RActionList::getAction(const QString &name)
{
    for (RAction *action : std::as_const(this->actions))
    {
        if (action != nullptr && action->objectName() == name)
        {
            return action;
        }
    }
    return nullptr;
}

QStringList RActionList::findGroupNames() const
{
    QStringList groupNames;

    for (const RAction *action : this->actions)
    {
        if (action)
        {
            const QString &groupName = action->getGroupName();
            if (!groupName.isEmpty() && !groupNames.contains(groupName))
            {
                groupNames.append(groupName);
            }

        }
    }

    groupNames.sort();

    return groupNames;
}

void RActionList::changeShortcut(uint position, const QString &shortcut)
{
    this->actions[position]->setShortcut(QKeySequence(shortcut));
}

void RActionList::setEnabled(bool enabled)
{
    this->setDisabled(!enabled);
}

void RActionList::setDisabled(bool disabled)
{
    for (RAction *action : std::as_const(this->actions))
    {
        if (action)
        {
            action->setDisabled(disabled);
        }
    }
}
