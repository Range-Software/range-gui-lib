#ifndef RGL_MESSAGE_BOX_H
#define RGL_MESSAGE_BOX_H

#include <QDialog>
#include <QDialogButtonBox>

#include <rbl_error.h>

class RMessageBox : public QDialog
{

    Q_OBJECT

    public:

        enum StandardButton
        {
            NoButton = 1 << 0,
            Ok       = 1 << 1,
            Yes      = 1 << 2,
            No       = 1 << 3
        };

    private:

        explicit RMessageBox(QWidget *parent,
                             const QIcon &icon,
                             const QString &title,
                             const QString &text,
                             QDialogButtonBox::StandardButtons buttons,
                             RError::Type errorType = RError::None,
                             const QString &errorMessage = QString());

    public:

        static RMessageBox::StandardButton critical(QWidget *parent, const QString &title, const QString &text, RError::Type errorType = RError::None, const QString &errorMessage = QString());

        static RMessageBox::StandardButton information(QWidget *parent, const QString &title, const QString &text);

        static RMessageBox::StandardButton question(QWidget *parent, const QString &title, const QString &text);

        static RMessageBox::StandardButton warning(QWidget *parent, const QString &title, const QString &text);

        static RMessageBox::StandardButton quit(QWidget *parent, const QString &text = QString());

};

#endif // RGL_MESSAGE_BOX_H
