#ifndef RGL_FILE_VERSION_DIALOG_H
#define RGL_FILE_VERSION_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>

#include <rcl_file_info.h>

class RFileVersionDialog : public QDialog
{
    Q_OBJECT

    protected:

        RVersion version;

        QPushButton *okButton;

    public:

        //! Constructor.
        explicit RFileVersionDialog(const RFileInfo &fileInfo, QWidget *parent = nullptr);

        //! Return version.
        RVersion getVersion() const;

    protected slots:

        //! Major version spin-box value changed.
        void onMajorValueChanged(int value);

        //! Minor version spin-box value changed.
        void onMinorValueChanged(int value);

        //! Release version spin-box value changed.
        void onReleaseValueChanged(int value);

};

#endif // RGL_FILE_VERSION_DIALOG_H
