#ifndef RGL_HELP_DIALOG_H
#define RGL_HELP_DIALOG_H

#include <QDialog>
#include <QTabWidget>

class RHelpDialog : public QDialog
{

    Q_OBJECT

    protected:

        QString helpPath;
        QString languageCode;

        QTabWidget *tabWidget;

    public:

        //! Constructor.
        explicit RHelpDialog(const QString &title, const QMap<QString, QString> &topics, QWidget *parent = nullptr);

    protected:

        QString findHelpFilePath(const QString &topic) const;

};

#endif // RGL_HELP_DIALOG_H
