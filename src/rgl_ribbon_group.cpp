#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

#include "rgl_ribbon_group.h"

//! Number of small buttons stacked in one column.
static constexpr int smallButtonsInColumn = 3;

RRibbonGroup::RRibbonGroup(const QString &title, QWidget *parent)
    : QWidget(parent)
    , nextColumn(0)
    , nextSmallRow(0)
    , largeIconSize(32)
    , smallIconSize(18)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2,0,2,0);
    mainLayout->setSpacing(0);

    this->buttonsLayout = new QGridLayout;
    this->buttonsLayout->setContentsMargins(0,0,0,0);
    this->buttonsLayout->setHorizontalSpacing(2);
    this->buttonsLayout->setVerticalSpacing(0);
    mainLayout->addLayout(this->buttonsLayout,1);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setEnabled(false);
    QFont titleFont(titleLabel->font());
    titleFont.setPointSizeF(titleFont.pointSizeF()*0.85);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
}

QToolButton *RRibbonGroup::addLargeAction(QAction *action, const QString &label)
{
    QToolButton *button = this->createActionButton(action,label);
    this->placeLargeButton(button);
    return button;
}

QToolButton *RRibbonGroup::addSmallAction(QAction *action, const QString &label)
{
    QToolButton *button = this->createActionButton(action,label);
    this->placeSmallButton(button);
    return button;
}

QToolButton *RRibbonGroup::addLargeSplitAction(QAction *action, const QList<QAction*> &menuActions, const QString &label)
{
    QToolButton *button = this->createActionButton(action,label);

    QMenu *menu = new QMenu(button);
    menu->addActions(menuActions);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::MenuButtonPopup);

    this->placeLargeButton(button);
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

    this->placeSmallButton(button);
    return button;
}

void RRibbonGroup::setIconSizes(int largeIconSize, int smallIconSize)
{
    this->largeIconSize = largeIconSize;
    this->smallIconSize = smallIconSize;

    for (QToolButton *button : std::as_const(this->largeButtons))
    {
        button->setIconSize(QSize(largeIconSize,largeIconSize));
    }
    for (QToolButton *button : std::as_const(this->smallButtons))
    {
        button->setIconSize(QSize(smallIconSize,smallIconSize));
    }
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

void RRibbonGroup::placeLargeButton(QToolButton *button)
{
    if (this->nextSmallRow > 0)
    {
        this->nextSmallRow = 0;
        this->nextColumn++;
    }

    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(QSize(this->largeIconSize,this->largeIconSize));
    button->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    this->buttonsLayout->addWidget(button,0,this->nextColumn++,smallButtonsInColumn,1);
    this->largeButtons.append(button);
}

void RRibbonGroup::placeSmallButton(QToolButton *button)
{
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIconSize(QSize(this->smallIconSize,this->smallIconSize));
    this->buttonsLayout->addWidget(button,this->nextSmallRow,this->nextColumn,Qt::AlignLeft);
    this->smallButtons.append(button);

    if (++this->nextSmallRow == smallButtonsInColumn)
    {
        this->nextSmallRow = 0;
        this->nextColumn++;
    }
}
