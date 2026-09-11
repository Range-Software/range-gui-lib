#ifndef RGL_ACTION_H
#define RGL_ACTION_H

#include <QAction>

#include <rbl_tool_input.h>

class RAction : public QAction
{

    Q_OBJECT

    public:

        typedef void (RAction::*PointerToMemberTrigger)();
        typedef void (RAction::*PointerToMemberToggle)(bool);

        struct Definition
        {
            bool separator;
            QString groupName;
            QString name;
            QString text;
            QString description;
            QString shortCut;
            QString icon;
            bool checkable;
            PointerToMemberTrigger triggerSlot;
            PointerToMemberToggle toggleSlot;
            QAction::MenuRole menuRole;
        };

    protected:

        bool separator;
        QString groupName;
        QString name;
        QString defaultShortCut;

    public:

        //! Constructor.
        explicit RAction(const RAction::Definition &actionDefinition, QObject *parent = nullptr);

        //! Create a new separator action.
        //!
        //! Every separator has to be an action of its own. QWidget::insertAction()
        //! drops an action which the widget already holds before inserting it again,
        //! so handing one and the same separator to a menu twice moves it to the end
        //! instead of adding a second one, leaving only the last separator visible.
        static RAction *createSeparator(QObject *parent = nullptr);

        //! Definition for separator action.
        static Definition definition();

        //! Definition for trigger action.
        static Definition definition(const QString &groupName,
                                     const QString &name,
                                     const QString &text,
                                     const QString &description,
                                     const QString &shortCut,
                                     const QString &icon,
                                     PointerToMemberTrigger triggerSlot,
                                     QAction::MenuRole menuRole = QAction::NoRole);

        //! Definition for toggle action.
        static Definition definition(const QString &groupName,
                                     const QString &name,
                                     const QString &text,
                                     const QString &description,
                                     const QString &shortCut,
                                     const QString &icon,
                                     PointerToMemberToggle toggleSlot,
                                     QAction::MenuRole menuRole = QAction::NoRole);

        //! Return true if action is separator.
        bool isSeparator() const;

        //! Return const reference to action group name.
        const QString &getGroupName() const;

        //! Return const reference to action name.
        const QString &getName() const;

        //! Return const reference to default sshortcut.
        const QString &getDefaultShortcut() const;

    protected:

        //! Execute tool action.
        void executeToolAction(const RToolInput &toolInput);

    public slots:

        //! Enable action.
        void enable();

        //! Disable action.
        void disable();

};

#endif // RGL_ACTION_H
