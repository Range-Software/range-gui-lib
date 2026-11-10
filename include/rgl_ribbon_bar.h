#ifndef RGL_RIBBON_BAR_H
#define RGL_RIBBON_BAR_H

#include <QIcon>
#include <QList>
#include <QString>
#include <QWidget>

#include "rgl_ribbon_group.h"

class QAction;
class QHBoxLayout;
class QMenu;
class QStackedWidget;
class QTabBar;
class QToolButton;

//! Ribbon with tabbed pages of grouped commands.
//!
//! Buttons only present given actions, so text, enabled state and shortcuts stay managed by the actions.
class RRibbonBar : public QWidget
{

    Q_OBJECT

    protected:

        //! Layout of items in front of tabs.
        QHBoxLayout *leadingLayout;
        //! Layout of items behind tabs.
        QHBoxLayout *trailingLayout;
        //! Tabs.
        QTabBar *tabBar;
        //! Pages.
        QStackedWidget *pages;
        //! Button collapsing pages.
        QToolButton *collapseButton;
        //! Groups on all pages.
        QList<RRibbonGroup*> groups;
        //! Buttons in the line with tabs.
        QList<QToolButton*> headerButtons;
        //! Size of icons on large buttons.
        int largeIconSize;
        //! True if the ribbon is compact.
        bool compact;
        //! True if pages are collapsed.
        bool collapsed;
        //! True if last click on a tab has expanded collapsed pages.
        bool expandedByTabClick;
        //! True if click on a tab is a part of double click which was already processed.
        bool ignoreTabClick;

    public:

        //! Constructor.
        explicit RRibbonBar(QWidget *parent = nullptr);

        //! Add text button opening given menu in front of tabs.
        void addLeadingMenu(const QString &label, QMenu *menu);

        //! Add icon button for given action in front of tabs.
        void addLeadingAction(QAction *action);

        //! Add icon button for given action behind tabs.
        void addTrailingAction(QAction *action);

        //! Add icon button opening given menu behind tabs.
        void addTrailingMenu(const QString &toolTip, const QIcon &icon, QMenu *menu);

        //! Add new page and return its position.
        int addPage(const QString &title);

        //! Add new group to page at given position.
        RRibbonGroup *addGroup(int pagePosition, const QString &title);

        //! Return position of current page.
        int getCurrentPage() const;

        //! Set page at given position as current.
        void setCurrentPage(int pagePosition);

        //! Return true if pages are collapsed.
        bool isCollapsed() const;

        //! Set whether pages are collapsed.
        void setCollapsed(bool collapsed);

        //! Set size of icons on large buttons, all other sizes follow it.
        void setIconSize(int iconSize);

        //! Return true if the ribbon is compact.
        bool isCompact() const;

        //! Set whether the ribbon is compact, which takes less vertical space.
        void setCompact(bool compact);

    protected:

        //! Widget has been resized.
        void resizeEvent(QResizeEvent *event) override;

        //! Return size of icons on small buttons.
        int findSmallIconSize() const;

        //! Create icon button for the line with tabs.
        QToolButton *createHeaderButton();

        //! Return vertical margin of pages.
        int findPageMargin() const;

        //! Refresh layouts of pages after their buttons have changed.
        void refreshPages();

        //! Update height of pages so that all of them fit and switching them does not move the view below.
        void updatePagesHeight();

    protected slots:

        //! Tab was clicked.
        void onTabBarClicked();

        //! Tab was double clicked.
        void onTabBarDoubleClicked();

};

#endif // RGL_RIBBON_BAR_H
