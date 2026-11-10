#include <algorithm>

#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>
#include <QTabBar>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "rgl_ribbon_bar.h"

RRibbonBar::RRibbonBar(QWidget *parent)
    : QWidget(parent)
    , largeIconSize(32)
    , collapsed(false)
    , expandedByTabClick(false)
    , ignoreTabClick(false)
{
    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(4,2,4,0);
    headerLayout->setSpacing(2);
    mainLayout->addLayout(headerLayout);

    this->leadingLayout = new QHBoxLayout;
    this->leadingLayout->setContentsMargins(0,0,0,0);
    this->leadingLayout->setSpacing(1);
    headerLayout->addLayout(this->leadingLayout);

    headerLayout->addSpacing(8);

    this->tabBar = new QTabBar(this);
    this->tabBar->setDocumentMode(true);
    this->tabBar->setDrawBase(false);
    this->tabBar->setExpanding(false);
    headerLayout->addWidget(this->tabBar);

    headerLayout->addStretch(1);

    this->trailingLayout = new QHBoxLayout;
    this->trailingLayout->setContentsMargins(0,0,0,0);
    this->trailingLayout->setSpacing(1);
    headerLayout->addLayout(this->trailingLayout);

    this->collapseButton = new QToolButton(this);
    this->collapseButton->setAutoRaise(true);
    this->collapseButton->setArrowType(Qt::UpArrow);
    this->collapseButton->setToolTip(tr("Collapse the ribbon"));
    headerLayout->addWidget(this->collapseButton);

    this->pages = new QStackedWidget(this);
    mainLayout->addWidget(this->pages);

    QFrame *bottomLine = new QFrame(this);
    bottomLine->setFrameShape(QFrame::HLine);
    bottomLine->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(bottomLine);

    QObject::connect(this->tabBar,&QTabBar::currentChanged,this->pages,&QStackedWidget::setCurrentIndex);
    QObject::connect(this->tabBar,&QTabBar::tabBarClicked,this,&RRibbonBar::onTabBarClicked);
    QObject::connect(this->tabBar,&QTabBar::tabBarDoubleClicked,this,&RRibbonBar::onTabBarDoubleClicked);
    QObject::connect(this->collapseButton,&QToolButton::clicked,this,[this]() { this->setCollapsed(!this->collapsed); });
}

void RRibbonBar::addLeadingMenu(const QString &label, QMenu *menu)
{
    QToolButton *button = new QToolButton(this);
    button->setText(label);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
    this->leadingLayout->addWidget(button);
}

void RRibbonBar::addLeadingAction(QAction *action)
{
    QToolButton *button = this->createHeaderButton();
    button->setDefaultAction(action);
    this->leadingLayout->addWidget(button);
}

void RRibbonBar::addTrailingAction(QAction *action)
{
    QToolButton *button = this->createHeaderButton();
    button->setDefaultAction(action);
    this->trailingLayout->addWidget(button);
}

void RRibbonBar::addTrailingMenu(const QString &toolTip, const QIcon &icon, QMenu *menu)
{
    QToolButton *button = this->createHeaderButton();
    button->setToolTip(toolTip);
    button->setIcon(icon);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
    this->trailingLayout->addWidget(button);
}

int RRibbonBar::addPage(const QString &title)
{
    QWidget *page = new QWidget;
    QHBoxLayout *pageLayout = new QHBoxLayout(page);
    pageLayout->setContentsMargins(4,2,4,2);
    pageLayout->setSpacing(4);
    pageLayout->addStretch(1);

    // Page which does not fit must scroll, otherwise it would dictate minimum width of the whole window.
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(page);

    const int pagePosition = this->pages->addWidget(scrollArea);
    this->tabBar->addTab(title);
    return pagePosition;
}

RRibbonGroup *RRibbonBar::addGroup(int pagePosition, const QString &title)
{
    QScrollArea *scrollArea = qobject_cast<QScrollArea*>(this->pages->widget(pagePosition));
    QWidget *page = scrollArea->widget();
    QHBoxLayout *pageLayout = qobject_cast<QHBoxLayout*>(page->layout());

    // Last item of the page is a stretch keeping groups on the left.
    int layoutPosition = pageLayout->count() - 1;
    if (layoutPosition > 0)
    {
        QFrame *separator = new QFrame(page);
        separator->setFrameShape(QFrame::VLine);
        separator->setFrameShadow(QFrame::Sunken);
        pageLayout->insertWidget(layoutPosition++,separator);
    }

    RRibbonGroup *group = new RRibbonGroup(title,page);
    group->setIconSizes(this->largeIconSize,this->findSmallIconSize());
    pageLayout->insertWidget(layoutPosition,group);
    this->groups.append(group);

    return group;
}

int RRibbonBar::getCurrentPage() const
{
    return this->tabBar->currentIndex();
}

void RRibbonBar::setCurrentPage(int pagePosition)
{
    if (pagePosition >= 0 && pagePosition < this->tabBar->count())
    {
        this->tabBar->setCurrentIndex(pagePosition);
    }
}

bool RRibbonBar::isCollapsed() const
{
    return this->collapsed;
}

void RRibbonBar::setCollapsed(bool collapsed)
{
    this->collapsed = collapsed;
    this->pages->setVisible(!collapsed);
    this->collapseButton->setArrowType(collapsed ? Qt::DownArrow : Qt::UpArrow);
    this->collapseButton->setToolTip(collapsed ? tr("Expand the ribbon") : tr("Collapse the ribbon"));
}

void RRibbonBar::setIconSize(int iconSize)
{
    this->largeIconSize = iconSize;

    const int smallIconSize = this->findSmallIconSize();
    for (RRibbonGroup *group : std::as_const(this->groups))
    {
        group->setIconSizes(iconSize,smallIconSize);
    }
    for (QToolButton *button : std::as_const(this->headerButtons))
    {
        button->setIconSize(QSize(smallIconSize,smallIconSize));
    }

    // Size hints of nested layouts are refreshed only when layouts are activated, starting from the innermost.
    for (RRibbonGroup *group : std::as_const(this->groups))
    {
        group->layout()->activate();
    }
    for (int i=0;i<this->pages->count();i++)
    {
        qobject_cast<QScrollArea*>(this->pages->widget(i))->widget()->layout()->activate();
    }

    this->updatePagesHeight();
}

void RRibbonBar::resizeEvent(QResizeEvent *event)
{
    this->QWidget::resizeEvent(event);
    if (event->size().width() != event->oldSize().width())
    {
        this->updatePagesHeight();
    }
}

int RRibbonBar::findSmallIconSize() const
{
    return std::max(16,this->largeIconSize*9/16);
}

QToolButton *RRibbonBar::createHeaderButton()
{
    const int smallIconSize = this->findSmallIconSize();

    QToolButton *button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIconSize(QSize(smallIconSize,smallIconSize));
    this->headerButtons.append(button);
    return button;
}

void RRibbonBar::updatePagesHeight()
{
    int height = 0;
    bool scrollBarNeeded = false;
    int scrollBarHeight = 0;

    for (int i=0;i<this->pages->count();i++)
    {
        const QScrollArea *scrollArea = qobject_cast<QScrollArea*>(this->pages->widget(i));
        const QSize pageSize = scrollArea->widget()->sizeHint();
        height = std::max(height,pageSize.height());
        scrollBarNeeded = scrollBarNeeded || pageSize.width() > this->width();
        scrollBarHeight = scrollArea->horizontalScrollBar()->sizeHint().height();
    }

    this->pages->setFixedHeight(height + (scrollBarNeeded ? scrollBarHeight : 0));
}

void RRibbonBar::onTabBarClicked()
{
    if (this->ignoreTabClick)
    {
        return;
    }

    this->expandedByTabClick = this->collapsed;
    if (this->collapsed)
    {
        this->setCollapsed(false);
    }
}

void RRibbonBar::onTabBarDoubleClicked()
{
    // Tab bar reports the second press of a double click as a click once more.
    this->ignoreTabClick = true;
    QTimer::singleShot(0,this,[this]() { this->ignoreTabClick = false; });

    // First press of this double click has already expanded the pages.
    if (!this->expandedByTabClick)
    {
        this->setCollapsed(!this->collapsed);
    }
    this->expandedByTabClick = false;
}
