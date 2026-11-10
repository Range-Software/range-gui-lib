#include <QAction>
#include <QMenu>

#include "rgl_split_tool_button.h"

RSplitToolButton::RSplitToolButton(const QList<QAction*> &actions, bool rememberLastUsed, QWidget *parent)
    : QToolButton(parent)
    , splitActions(actions)
    , rememberLastUsed(rememberLastUsed)
    , preferredPosition(0)
{
    this->setAutoRaise(true);
    this->setFocusPolicy(Qt::NoFocus);

    QMenu *menu = new QMenu(this);
    menu->addActions(actions);
    this->setMenu(menu);
    this->setPopupMode(QToolButton::MenuButtonPopup);

    QObject::connect(menu,&QMenu::triggered,this,&RSplitToolButton::onMenuTriggered);
    for (QAction *action : actions)
    {
        QObject::connect(action,&QAction::changed,this,&RSplitToolButton::updateDefaultAction);
    }

    this->updateDefaultAction();
}

bool RSplitToolButton::getRememberLastUsed() const
{
    return this->rememberLastUsed;
}

int RSplitToolButton::getLastUsedPosition() const
{
    return this->preferredPosition;
}

void RSplitToolButton::setLastUsedPosition(int position)
{
    if (this->rememberLastUsed && position >= 0 && position < this->splitActions.size())
    {
        this->preferredPosition = position;
        this->updateDefaultAction();
    }
}

void RSplitToolButton::updateDefaultAction()
{
    QAction *action = this->splitActions.at(this->preferredPosition);

    if (!action->isEnabled())
    {
        for (QAction *otherAction : std::as_const(this->splitActions))
        {
            if (otherAction->isEnabled())
            {
                action = otherAction;
                break;
            }
        }
    }

    this->setDefaultAction(action);
}

void RSplitToolButton::onMenuTriggered(QAction *action)
{
    if (this->rememberLastUsed)
    {
        this->setLastUsedPosition(this->splitActions.indexOf(action));
    }
}
