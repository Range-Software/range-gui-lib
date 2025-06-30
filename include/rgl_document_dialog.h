#ifndef RGL_DOCUMENT_DIALOG_H
#define RGL_DOCUMENT_DIALOG_H

#include <QDialog>
#include <QMap>
#include <QTabWidget>

class RDocumentDialog : public QDialog
{

    Q_OBJECT

    protected:

        //! Tab widget.
        QTabWidget *tabWidget;

    public:

        //! Constructor.
        explicit RDocumentDialog(const QString &title, QMap<QString,QString> helpPaths, QWidget *parent = nullptr);

};

#endif // RGL_DOCUMENT_DIALOG_H
