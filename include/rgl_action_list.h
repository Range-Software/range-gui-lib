#ifndef RGL_ACTION_LIST_H
#define RGL_ACTION_LIST_H

#include <QObject>

#include <rcl_cloud_session_manager.h>

#include "rgl_action.h"

class RActionList : public QObject
{
    Q_OBJECT

    protected:

        //! List of actions.
        QList<RAction*> actions;

    public:

        //! Constructor.
        explicit RActionList(const QList<RAction::Definition> &definitions, QObject *parent = nullptr);

        //! Return number of actions.
        uint getNActions() const;

        //! Return const action for given action type.
        const RAction *getAction(uint position) const;

        //! Return action for given action type.
        RAction *getAction(uint position);

        //! Return action for given action name.
        RAction *getAction(const QString &name);

        //! Return list of group names.
        QStringList findGroupNames() const;

    public slots:

        //! Change action shortcut for given action type.
        void changeShortcut(uint position, const QString &shortcut);

        //! Enable disable actions.
        void setEnabled(bool enabled);

        //! Enable disable actions.
        void setDisabled(bool disabled);

};

#endif /* RGL_ACTION_LIST_H */
