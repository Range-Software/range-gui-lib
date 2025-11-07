#ifndef RGL_DOCUMENT_WIDGET_H
#define RGL_DOCUMENT_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QTextBrowser>

class RDocumentWidget : public QWidget
{

    Q_OBJECT

    protected:

        //! List widget.
        QListWidget *listWidget;
        //! Text browser.
        QTextBrowser *textBrowser;
        //! Default document.
        QString defaultFileName;

    public:

        //! Constructor
        explicit RDocumentWidget(const QString &searchPath, const QString &defaultFileName = "index.md", QWidget *parent = nullptr);

    protected:

        //! Add list item.
        void addListItem(QIcon icon, const QString &text, const QString &fileName);

        //! Add list item.
        void addListItem(const QString &text, const QString &fileName);

        //! Load file.
        void loadFile(const QString &fileName);

    private slots:

        //! List selection changed
        void onListSelectionChanged();

};

#endif // RGL_DOCUMENT_WIDGET_H
