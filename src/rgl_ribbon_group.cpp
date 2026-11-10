#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

#include "rgl_ribbon_group.h"

//! Number of small buttons stacked in one column.
static constexpr int smallButtonsInColumn = 3;
//! Number of small buttons stacked in one column of compact group.
static constexpr int compactButtonsInColumn = 2;

RRibbonGroup::RRibbonGroup(const QString &title, QWidget *parent)
    : QWidget(parent)
    , largeIconSize(32)
    , smallIconSize(18)
    , compact(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2,0,2,0);
    mainLayout->setSpacing(0);

    this->buttonsLayout = new QGridLayout;
    this->buttonsLayout->setContentsMargins(0,0,0,0);
    this->buttonsLayout->setHorizontalSpacing(2);
    this->buttonsLayout->setVerticalSpacing(0);
    mainLayout->addLayout(this->buttonsLayout,1);

    this->titleLabel = new QLabel(title);
    this->titleLabel->setAlignment(Qt::AlignCenter);
    this->titleLabel->setEnabled(false);
    QFont titleFont(this->titleLabel->font());
    titleFont.setPointSizeF(titleFont.pointSizeF()*0.85);
    this->titleLabel->setFont(titleFont);
    mainLayout->addWidget(this->titleLabel);
}

QToolButton *RRibbonGroup::addLargeAction(QAction *action, const QString &label)
{
    QToolButton *button = this->createActionButton(action,label);
    this->addButton(button,true);
    return button;
}

QToolButton *RRibbonGroup::addSmallAction(QAction *action, const QString &label)
{
    QToolButton *button = this->createActionButton(action,label);
    this->addButton(button,false);
    return button;
}

QToolButton *RRibbonGroup::addLargeSplitAction(QAction *action, const QList<QAction*> &menuActions, const QString &label)
{
    QToolButton *button = this->createActionButton(action,label);

    QMenu *menu = new QMenu(button);
    menu->addActions(menuActions);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::MenuButtonPopup);

    this->addButton(button,true);
    return button;
}

QToolButton *RRibbonGroup::addSmallMenu(const QString &label, const QIcon &icon, QMenu *menu)
{
    QToolButton *button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setText(label);
    button->setIcon(icon);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);

    this->addButton(button,false);
    return button;
}

void RRibbonGroup::setIconSizes(int largeIconSize, int smallIconSize)
{
    this->largeIconSize = largeIconSize;
    this->smallIconSize = smallIconSize;
    this->arrangeButtons();
}

void RRibbonGroup::setCompact(bool compact)
{
    this->compact = compact;
    this->titleLabel->setVisible(!compact);
    this->arrangeButtons();
}

QToolButton *RRibbonGroup::createActionButton(QAction *action, const QString &label)
{
    if (!label.isEmpty())
    {
        // Icon text is used only by buttons, menus keep showing full text of the action.
        action->setIconText(label);
    }

    QToolButton *button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(action);
    return button;
}

void RRibbonGroup::addButton(QToolButton *button, bool large)
{
    this->items.append({button,large});
    this->arrangeButtons();
}

void RRibbonGroup::arrangeButtons()
{
    for (const Item &item : std::as_const(this->items))
    {
        this->buttonsLayout->removeWidget(item.button);
    }

    const int buttonsInColumn = this->compact ? compactButtonsInColumn : smallButtonsInColumn;
    int column = 0;
    int row = 0;

    for (const Item &item : std::as_const(this->items))
    {
        if (item.large && !this->compact)
        {
            // Large button starts a new column.
            if (row > 0)
            {
                row = 0;
                column++;
            }

            item.button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            item.button->setIconSize(QSize(this->largeIconSize,this->largeIconSize));
            item.button->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
            this->buttonsLayout->addWidget(item.button,0,column++,buttonsInColumn,1);
        }
        else
        {
            item.button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            item.button->setIconSize(QSize(this->smallIconSize,this->smallIconSize));
            item.button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            this->buttonsLayout->addWidget(item.button,row,column,Qt::AlignLeft);

            if (++row == buttonsInColumn)
            {
                row = 0;
                column++;
            }
        }
    }
}
