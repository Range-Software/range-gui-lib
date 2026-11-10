#ifndef RGL_SPLIT_TOOL_BUTTON_H
#define RGL_SPLIT_TOOL_BUTTON_H

#include <QList>
#include <QToolButton>

class QAction;

//! Tool button triggering one of given actions, the others are available from its drop-down list.
class RSplitToolButton : public QToolButton
{

    Q_OBJECT

    protected:

        //! Actions in the drop-down list.
        QList<QAction*> splitActions;
        //! True if the last triggered action becomes the action of the button.
        bool rememberLastUsed;
        //! Position of action which the button should trigger.
        int preferredPosition;

    public:

        //! Constructor.
        //! Button triggers the first action unless last used action is to be remembered.
        explicit RSplitToolButton(const QList<QAction*> &actions, bool rememberLastUsed, QWidget *parent = nullptr);

        //! Return true if the last triggered action becomes the action of the button.
        bool getRememberLastUsed() const;

        //! Return position of last used action.
        int getLastUsedPosition() const;

        //! Set position of last used action.
        void setLastUsedPosition(int position);

    protected:

        //! Show preferred action, or the first enabled one so that the drop-down list stays reachable.
        void updateDefaultAction();

    protected slots:

        //! Action from the drop-down list was triggered, no matter whether from the list, a shortcut or elsewhere.
        void onMenuTriggered(QAction *action);

};

#endif // RGL_SPLIT_TOOL_BUTTON_H
