#ifndef RGL_DOCUMENT_WIDGET_H
#define RGL_DOCUMENT_WIDGET_H

#include <QHash>
#include <QMap>
#include <QSizeF>
#include <QString>
#include <QTimer>
#include <QUrl>
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
        //! Document which is currently shown.
        //! Relative links are resolved against its directory.
        QString currentFileName;
        //! Whether the document which is currently shown came from Markdown.
        bool markdownDocument;
        //! Position of every heading of the current Markdown document, keyed by
        //! the anchor a link would use to reach it. Markdown headings carry no
        //! named anchor of their own, so the links pointing at them have to be
        //! resolved here.
        QMap<QString,int> anchorPositions;
        //! Whether one list item has to be selected at all times.
        bool selectionRequired;
        //! Row of the list item which was selected last.
        int lastSelectedRow;
        //! Natural size of every image of the current document, keyed by the
        //! name the document refers to it by.
        QHash<QString,QSizeF> imageSizes;
        //! Width the images of the current document were fitted to last.
        qreal fittedImageWidth;
        //! Timer delaying the image fitting until resizing settles.
        QTimer *fitImagesTimer;

    public:

        //! Constructor
        explicit RDocumentWidget(const QString &searchPath, const QString &defaultFileName = "index.md", QWidget *parent = nullptr);

        //! Return the anchor a link would use to reach a heading of given text.
        //! Lower-cased, punctuation dropped and spaces turned into hyphens -
        //! the convention the usual Markdown renderers follow.
        static QString anchorId(const QString &text);

        //! Require exactly one list item to be selected at all times.
        //! When enabled and nothing is selected yet, the item showing the
        //! current document is selected, or the first one if there is none.
        void setSelectionRequired(bool selectionRequired);

    protected:

        //! Add list item.
        void addListItem(QIcon icon, const QString &text, const QString &fileName);

        //! Add list item.
        void addListItem(const QString &text, const QString &fileName);

        //! Load file.
        void loadFile(const QString &fileName);

        //! Find the headings of the current Markdown document and store the
        //! anchor of each one.
        void findAnchors();

        //! Scale every image of the current document wider than the text
        //! browser down to its width, and every other back to its natural size.
        void fitImages();

        //! Return the natural size of given image of the current document.
        //! An empty size is returned if the image cannot be loaded.
        QSizeF findImageSize(const QString &name);

        //! Filter events of the text browser viewport.
        bool eventFilter(QObject *watched, QEvent *event) override;

        //! Scroll the text browser to given anchor of the current document.
        //! An empty anchor scrolls to the top. Returns false if the document
        //! has no such anchor.
        bool scrollToDocumentAnchor(const QString &anchor);

        //! Select the list item pointing to given document, without reloading
        //! it. Every item is deselected if none points there.
        void selectListItem(const QString &fileName);

    private slots:

        //! List selection changed
        void onListSelectionChanged();

        //! Link in the document was clicked.
        void onAnchorClicked(const QUrl &url);

};

#endif // RGL_DOCUMENT_WIDGET_H
