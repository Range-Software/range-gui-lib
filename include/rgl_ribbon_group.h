#ifndef RGL_RIBBON_GROUP_H
#define RGL_RIBBON_GROUP_H

#include <QIcon>
#include <QList>
#include <QString>
#include <QWidget>

class QAction;
class QGridLayout;
class QMenu;
class QToolButton;

//! Titled group of buttons on a ribbon page.
//!
//! Large buttons take the whole height of the group, small buttons are stacked into columns.
class RRibbonGroup : public QWidget
{

    Q_OBJECT

    protected:

        //! Layout holding buttons.
        QGridLayout *buttonsLayout;
        //! Column for next button.
        int nextColumn;
        //! Row for next small button.
        int nextSmallRow;
        //! Size of icons on large buttons.
        int largeIconSize;
        //! Size of icons on small buttons.
        int smallIconSize;
        //! Large buttons.
        QList<QToolButton*> largeButtons;
        //! Small buttons.
        QList<QToolButton*> smallButtons;

    public:

        //! Constructor.
        explicit RRibbonGroup(const QString &title, QWidget *parent = nullptr);

        //! Add large button for given action.
        //! Label, if given, replaces text of the action on the button.
        QToolButton *addLargeAction(QAction *action, const QString &label = QString());

        //! Add small button for given action.
        //! Label, if given, replaces text of the action on the button.
        QToolButton *addSmallAction(QAction *action, const QString &label = QString());

        //! Add large button triggering given action with a drop-down list of given actions.
        QToolButton *addLargeSplitAction(QAction *action, const QList<QAction*> &menuActions, const QString &label = QString());

        //! Add small button opening given menu.
        QToolButton *addSmallMenu(const QString &label, const QIcon &icon, QMenu *menu);

        //! Set size of icons.
        void setIconSizes(int largeIconSize, int smallIconSize);

    protected:

        //! Create button for given action.
        QToolButton *createActionButton(QAction *action, const QString &label);

        //! Place button as a large one.
        void placeLargeButton(QToolButton *button);

        //! Place button as a small one.
        void placeSmallButton(QToolButton *button);

};

#endif // RGL_RIBBON_GROUP_H
