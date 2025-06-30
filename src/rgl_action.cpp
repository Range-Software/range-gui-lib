#include <rbl_job_manager.h>
#include <rbl_tool_task.h>

#include "rgl_action.h"

RAction::RAction(const Definition &actionDefinition, QObject *parent)
    : QAction{parent}
{
    this->separator = actionDefinition.separator;
    this->groupName = actionDefinition.groupName;
    this->name = actionDefinition.name;
    this->defaultShortCut = actionDefinition.shortCut;

    this->setSeparator(this->separator);

    if (!this->separator)
    {
        this->setText(actionDefinition.text);
        this->setToolTip(actionDefinition.description);
        this->setShortcut(actionDefinition.shortCut);
        if (!actionDefinition.icon.isEmpty())
        {
            this->setIcon(QIcon(actionDefinition.icon));
            this->setIconVisibleInMenu(true);
        }
        this->setMenuRole(actionDefinition.menuRole);
        if (actionDefinition.checkable)
        {
            this->setCheckable(true);
            QObject::connect(this, &RAction::toggled, this, actionDefinition.toggleSlot);
        }
        else
        {
            QObject::connect(this, &RAction::triggered, this, actionDefinition.triggerSlot);
        }
    }
}

RAction::Definition RAction::definition()
{
    return {
        true,
        QString(),
        QString(),
        QString(),
        QString(),
        QString(),
        QString(),
        false,
        nullptr,
        nullptr,
        QAction::NoRole
    };
}

RAction::Definition RAction::definition(const QString &groupName,
                                        const QString &name,
                                        const QString &text,
                                        const QString &description,
                                        const QString &shortCut,
                                        const QString &icon,
                                        PointerToMemberTrigger triggerSlot,
                                        MenuRole menuRole)
{
    return {
        false,
        groupName,
        name,
        text,
        description,
        shortCut,
        icon,
        false,
        triggerSlot,
        nullptr,
        menuRole
    };
}

RAction::Definition RAction::definition(const QString &groupName,
                                        const QString &name,
                                        const QString &text,
                                        const QString &description,
                                        const QString &shortCut,
                                        const QString &icon,
                                        PointerToMemberToggle toggleSlot,
                                        MenuRole menuRole)
{
    return {
        false,
        groupName,
        name,
        text,
        description,
        shortCut,
        icon,
        true,
        nullptr,
        toggleSlot,
        menuRole
    };
}

bool RAction::isSeparator() const
{
    return this->separator;
}

const QString &RAction::getGroupName() const
{
    return this->groupName;
}

const QString &RAction::getName() const
{
    return this->name;
}

const QString &RAction::getDefaultShortcut() const
{
    return this->defaultShortCut;
}

void RAction::executeToolAction(const RToolInput &toolInput)
{
    RToolTask *toolTask = new RToolTask(toolInput);
    RJobManager::getInstance().submit(toolTask);
}

void RAction::enable()
{
    this->setEnabled(true);
}

void RAction::disable()
{
    this->setDisabled(true);
}
