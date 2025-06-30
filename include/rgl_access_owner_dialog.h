#ifndef RGL_ACCESS_OWNER_DIALOG_H
#define RGL_ACCESS_OWNER_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>

#include <rcl_file_info.h>

class RAccessOwnerDialog : public QDialog
{

    Q_OBJECT

    protected:

        RAccessOwner accessOwner;

        QPushButton *okButton;

    public:

        //! Constructor.
        explicit RAccessOwnerDialog(const QUuid &id, const QString &name, const RAccessOwner &accessOwner, QWidget *parent = nullptr);

        //! Return access owner.
        RAccessOwner getAccessOwner() const;

    protected slots:

        //! User has changed.
        void onUserChanged(const QString &text);

        //! Group has changed.
        void onGroupChanged(const QString &text);

};

#endif // RGL_ACCESS_OWNER_DIALOG_H
