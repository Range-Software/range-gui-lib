#ifndef RGL_HTML_VIEWER_H
#define RGL_HTML_VIEWER_H

#include <QTextBrowser>

class RHtmlViewer : public QTextBrowser
{

    Q_OBJECT

    public:

        //! Construtor.
        explicit RHtmlViewer(QWidget *parent = nullptr);

    protected:

        //! Create custom popup menu.
        void contextMenuEvent(QContextMenuEvent *event);

    protected slots:

        void onShowSource();

};

#endif // RGL_HTML_VIEWER_H
