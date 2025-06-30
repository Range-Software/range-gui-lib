#ifndef RGL_ACCESS_MODE_DIALOG_H
#define RGL_ACCESS_MODE_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>

#include <rcl_file_info.h>

class RAccessModeDialog : public QDialog
{

    Q_OBJECT

    protected:

        enum CloudAccess
        {
            None = 0,
            Read,
            Write,
            Execute,
            ReadWrite,
            ReadExecute,
            WriteExecute,
            ReadWriteExecute,
            NumberOfModes
        };

        RAccessMode accessMode;

        QList<CloudAccess> validAccesses;

        QPushButton *okButton;

    public:

        //! Constructor.
        explicit RAccessModeDialog(const QUuid &id, const QString &name, const RAccessMode &accessMode, RAccessMode::ModeMask filter, QWidget *parent = nullptr);

        //! Return access mode.
        RAccessMode getAccessMode() const;

    protected:

        qsizetype findAccessComboIndex(RAccessMode::ModeMask modeMask) const;

        static QString getModeName(CloudAccess cloudAccess);

        static RAccessMode::ModeMask getModeMask(CloudAccess cloudAccess);

        static CloudAccess accessModeMaskToCloudAccess(RAccessMode::ModeMask modeMask);

        static QList<CloudAccess> accessModeMaskToCloudAccessList(RAccessMode::ModeMask modeMask);

        static qsizetype findPositionInCloudAccessList(CloudAccess access, const QList<CloudAccess> &accessList);

    protected slots:

        void onUserComboIndexChanged(int index);

        void onGroupComboIndexChanged(int index);

        void onOtherComboIndexChanged(int index);

};

#endif // RGL_ACCESS_MODE_DIALOG_H
