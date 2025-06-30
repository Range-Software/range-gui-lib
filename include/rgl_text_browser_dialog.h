#ifndef RGL_TEXT_BROWSER_DIALOG_H
#define RGL_TEXT_BROWSER_DIALOG_H

#include <QDialog>

class RTextBrowserDialog : public QDialog
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RTextBrowserDialog(const QString &title,
                                    const QString &desc,
                                    const QString &text,
                                    QWidget *parent = nullptr);

};

#endif // RGL_TEXT_BROWSER_DIALOG_H
