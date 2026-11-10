#ifndef RGL_RIBBON_GROUP_H
#define RGL_RIBBON_GROUP_H

#include <QIcon>
#include <QList>
#include <QString>
#include <QWidget>

class QAction;
class QGridLayout;
class QLabel;
class QMenu;
class QToolButton;

//! Titled group of buttons on a ribbon page.
//!
//! Large buttons take the whole height of the group, small buttons are stacked into columns.
//! In compact mode all buttons are small, they are stacked lower and the title is hidden.
class RRibbonGroup : public QWidget
{

    Q_OBJECT

    protected:

        //! Button of the group.
        struct Item
        {
            //! Button.
            QToolButton *button;
            //! True if button is large when the group is not compact.
            bool large;
        };

        //! Layout holding buttons.
        QGridLayout *buttonsLayout;
        //! Title of the group.
        QLabel *titleLabel;
        //! Buttons in order of their addition.
        QList<Item> items;
        //! Size of icons on large buttons.
        int largeIconSize;
        //! Size of icons on small buttons.
        int smallIconSize;
        //! True if the group is compact.
        bool compact;

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

        //! Set whether the group is compact.
        void setCompact(bool compact);

    protected:

        //! Create button for given action.
        QToolButton *createActionButton(QAction *action, const QString &label);

        //! Add button to the group.
        void addButton(QToolButton *button, bool large);

        //! Place all buttons according to the current mode.
        void arrangeButtons();

};

#endif // RGL_RIBBON_GROUP_H
