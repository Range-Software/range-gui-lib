#include <algorithm>
#include <cmath>

#include <QSplitter>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextFragment>
#include <QTextImageFormat>

#include <rbl_logger.h>

#include "rgl_document_widget.h"
#include "rgl_message_box.h"

RDocumentWidget::RDocumentWidget(const QString &searchPath, const QString &defaultFileName, QWidget *parent)
    : QWidget{parent}
    , markdownDocument{false}
    , selectionRequired{false}
    , lastSelectedRow{-1}
    , fittedImageWidth{-1.0}
{
    if (!defaultFileName.isEmpty())
    {
        this->defaultFileName = searchPath + QDir::separator() + defaultFileName;
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    this->listWidget = new QListWidget;
    this->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    this->listWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    splitter->addWidget(this->listWidget);

    QObject::connect(this->listWidget,&QListWidget::itemSelectionChanged,this,&RDocumentWidget::onListSelectionChanged);

    this->textBrowser = new QTextBrowser;
    this->textBrowser->setSearchPaths(QStringList() << searchPath);
    this->textBrowser->setReadOnly(true);
    this->textBrowser->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
    // The documents are handed to the browser as content and not as a source,
    // so it can resolve neither a relative link nor a Markdown heading anchor
    // on its own. Every link is followed here instead.
    this->textBrowser->setOpenLinks(false);
    splitter->addWidget(this->textBrowser);

    QObject::connect(this->textBrowser,&QTextBrowser::anchorClicked,this,&RDocumentWidget::onAnchorClicked);

    // Images are refitted once the viewport stops changing its size, so that
    // dragging the splitter does not relayout the document on every pixel.
    this->fitImagesTimer = new QTimer(this);
    this->fitImagesTimer->setSingleShot(true);
    this->fitImagesTimer->setInterval(50);
    QObject::connect(this->fitImagesTimer,&QTimer::timeout,this,&RDocumentWidget::fitImages);
    this->textBrowser->viewport()->installEventFilter(this);

    splitter->setStretchFactor(1,1);

    this->loadFile(this->defaultFileName);
}

QString RDocumentWidget::anchorId(const QString &text)
{
    QString id;
    id.reserve(text.size());

    for (const QChar &character : text)
    {
        if (character.isLetterOrNumber())
        {
            id.append(character.toLower());
        }
        else if (character == QLatin1Char('-') || character == QLatin1Char('_'))
        {
            id.append(character);
        }
        else if (character.isSpace())
        {
            id.append(QLatin1Char('-'));
        }
    }

    return id;
}

void RDocumentWidget::setSelectionRequired(bool selectionRequired)
{
    this->selectionRequired = selectionRequired;

    if (!this->selectionRequired || this->listWidget->count() == 0 || !this->listWidget->selectedItems().isEmpty())
    {
        return;
    }

    int row = 0;
    const QString filePath(QFileInfo(this->currentFileName).absoluteFilePath());
    for (int i=0;i<this->listWidget->count() && !this->currentFileName.isEmpty();i++)
    {
        if (QFileInfo(this->listWidget->item(i)->data(Qt::UserRole).toString()).absoluteFilePath() == filePath)
        {
            row = i;
            break;
        }
    }

    // Selecting through the current item loads its document.
    this->listWidget->setCurrentRow(row);
}

void RDocumentWidget::addListItem(QIcon icon, const QString &text, const QString &fileName)
{
    QListWidgetItem *item = new QListWidgetItem(this->listWidget);
    item->setIcon(icon);
    item->setText(text);
    item->setData(Qt::UserRole,fileName);
}

void RDocumentWidget::addListItem(const QString &text, const QString &fileName)
{
    QListWidgetItem *item = new QListWidgetItem(this->listWidget);
    item->setText(text);
    item->setData(Qt::UserRole,fileName);
}

void RDocumentWidget::loadFile(const QString &fileName)
{
    this->currentFileName.clear();
    this->markdownDocument = false;
    this->anchorPositions.clear();
    this->imageSizes.clear();
    this->fittedImageWidth = -1.0;

    if (fileName.isEmpty())
    {
        this->textBrowser->clear();
    }
    else
    {
        QFileInfo fileInfo(fileName);
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            RLogger::warning("Failed to load document file \"%s\".\n",fileName.toUtf8().constData());
            QString title = tr("No document file available");
            QString text = tr("Failed to load document file") + " <i>" + fileName + "</i>";
            RMessageBox::warning(this,title,text);
            this->textBrowser->setHtml(title + ".");
        }
        else
        {
            QTextStream in(&file);
            QString fileContent = in.readAll();
            if (fileInfo.suffix().toLower() == "md")
            {
                this->textBrowser->setMarkdown(fileContent);
                this->markdownDocument = true;
            }
            else if (fileInfo.suffix().toLower() == "html" ||
                     fileInfo.suffix().toLower() == "htm")
            {
                this->textBrowser->setHtml(fileContent);
            }
            else
            {
                this->textBrowser->setText(fileContent);
            }
            file.close();

            this->currentFileName = fileInfo.absoluteFilePath();

            if (this->markdownDocument)
            {
                this->findAnchors();
            }

            this->fitImages();
        }
    }
}

void RDocumentWidget::findAnchors()
{
    this->anchorPositions.clear();

    const QTextDocument *document = this->textBrowser->document();

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next())
    {
        if (block.blockFormat().headingLevel() <= 0)
        {
            continue;
        }

        const QString id = RDocumentWidget::anchorId(block.text());
        if (id.isEmpty())
        {
            continue;
        }

        // Headings repeating the same text are numbered the way the usual
        // Markdown renderers number them, so that the second one can be linked.
        QString uniqueId(id);
        for (uint i=1;this->anchorPositions.contains(uniqueId);i++)
        {
            uniqueId = id + QLatin1Char('-') + QString::number(i);
        }

        this->anchorPositions.insert(uniqueId,block.position());
    }
}

void RDocumentWidget::fitImages()
{
    QTextDocument *document = this->textBrowser->document();

    // A few pixels are spared so that an image as wide as the viewport does
    // not bring up the horizontal scroll bar.
    const qreal availableWidth = std::floor(this->textBrowser->viewport()->width() - 2.0 * document->documentMargin() - 4.0);
    if (availableWidth <= 0.0 || availableWidth == this->fittedImageWidth)
    {
        return;
    }
    this->fittedImageWidth = availableWidth;

    // Fragments are collected first, as changing their format while walking
    // the blocks would invalidate the iterators.
    QList<QTextFragment> imageFragments;
    for (QTextBlock block = document->begin(); block.isValid(); block = block.next())
    {
        for (QTextBlock::iterator iterator = block.begin(); !iterator.atEnd(); ++iterator)
        {
            const QTextFragment fragment = iterator.fragment();
            if (fragment.isValid() && fragment.charFormat().isImageFormat())
            {
                imageFragments.append(fragment);
            }
        }
    }

    if (imageFragments.isEmpty())
    {
        return;
    }

    QTextCursor cursor(document);
    cursor.beginEditBlock();
    for (const QTextFragment &fragment : std::as_const(imageFragments))
    {
        QTextImageFormat format = fragment.charFormat().toImageFormat();

        const QSizeF imageSize = this->findImageSize(format.name());
        if (imageSize.isEmpty())
        {
            continue;
        }

        // With the height left out the image keeps its aspect ratio.
        const qreal width = std::min(imageSize.width(),availableWidth);
        if (format.width() == width && !format.hasProperty(QTextFormat::ImageHeight))
        {
            continue;
        }
        format.setWidth(width);
        format.clearProperty(QTextFormat::ImageHeight);

        cursor.setPosition(fragment.position());
        cursor.setPosition(fragment.position() + fragment.length(),QTextCursor::KeepAnchor);
        cursor.setCharFormat(format);
    }
    cursor.endEditBlock();
}

QSizeF RDocumentWidget::findImageSize(const QString &name)
{
    const auto iterator = this->imageSizes.constFind(name);
    if (iterator != this->imageSizes.constEnd())
    {
        return iterator.value();
    }

    // The browser hands over images it loads from a file as raw data.
    const QVariant resource = this->textBrowser->document()->resource(QTextDocument::ImageResource,QUrl(name));

    QSizeF imageSize;
    if (resource.typeId() == QMetaType::QImage)
    {
        imageSize = resource.value<QImage>().deviceIndependentSize();
    }
    else if (resource.typeId() == QMetaType::QPixmap)
    {
        imageSize = resource.value<QPixmap>().deviceIndependentSize();
    }
    else if (resource.typeId() == QMetaType::QByteArray)
    {
        imageSize = QImage::fromData(resource.toByteArray()).deviceIndependentSize();
    }

    this->imageSizes.insert(name,imageSize);

    return imageSize;
}

bool RDocumentWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->textBrowser->viewport() && event->type() == QEvent::Resize)
    {
        this->fitImagesTimer->start();
    }
    return QWidget::eventFilter(watched,event);
}

bool RDocumentWidget::scrollToDocumentAnchor(const QString &anchor)
{
    if (anchor.isEmpty())
    {
        QScrollBar *scrollBar = this->textBrowser->verticalScrollBar();
        scrollBar->setValue(scrollBar->minimum());
        return true;
    }

    if (!this->markdownDocument)
    {
        // An HTML document carries its own named anchors and the browser
        // resolves them itself.
        this->textBrowser->scrollToAnchor(anchor);
        return true;
    }

    const auto iterator = this->anchorPositions.constFind(anchor);
    if (iterator == this->anchorPositions.constEnd())
    {
        return false;
    }

    QTextCursor cursor(this->textBrowser->document());
    cursor.setPosition(iterator.value());

    // Moving to the end of the document first leaves the heading at the top of
    // the viewport rather than at its bottom edge.
    this->textBrowser->moveCursor(QTextCursor::End);
    this->textBrowser->setTextCursor(cursor);

    return true;
}

void RDocumentWidget::selectListItem(const QString &fileName)
{
    const QString filePath(QFileInfo(fileName).absoluteFilePath());

    int row = -1;
    for (int i=0;i<this->listWidget->count();i++)
    {
        const QString itemFilePath(QFileInfo(this->listWidget->item(i)->data(Qt::UserRole).toString()).absoluteFilePath());
        if (itemFilePath == filePath)
        {
            row = i;
            break;
        }
    }

    // A document no item points to leaves a required selection where it is.
    if (row < 0 && this->selectionRequired)
    {
        return;
    }

    QSignalBlocker blocker(this->listWidget);

    for (int i=0;i<this->listWidget->count();i++)
    {
        this->listWidget->item(i)->setSelected(i == row);
    }
    if (row >= 0)
    {
        this->lastSelectedRow = row;
    }
}

void RDocumentWidget::onListSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = this->listWidget->selectedItems();

    if (selectedItems.size() == 0)
    {
        if (this->selectionRequired && this->lastSelectedRow >= 0 && this->lastSelectedRow < this->listWidget->count())
        {
            // Deselecting - a Ctrl+click on the selected item - is undone, and
            // the document which is shown stays.
            QSignalBlocker blocker(this->listWidget);
            this->listWidget->item(this->lastSelectedRow)->setSelected(true);
            return;
        }
        this->lastSelectedRow = -1;
        this->loadFile(this->defaultFileName);
    }
    else
    {
        this->lastSelectedRow = this->listWidget->row(selectedItems.at(0));
        this->loadFile(selectedItems.at(0)->data(Qt::UserRole).toString());
    }
}

void RDocumentWidget::onAnchorClicked(const QUrl &url)
{
    if (url.isEmpty())
    {
        return;
    }

    // Anything carrying a scheme of its own - http, https, mailto - leads out
    // of the document set and belongs to whichever application the desktop has
    // registered for it.
    if (!url.scheme().isEmpty() && url.scheme().compare(QLatin1String("file"),Qt::CaseInsensitive) != 0)
    {
        if (!QDesktopServices::openUrl(url))
        {
            RLogger::warning("Failed to open the link \"%s\" in an external application.\n",
                             url.toString().toUtf8().constData());
        }
        return;
    }

    const QString path(url.scheme().isEmpty() ? url.path() : url.toLocalFile());
    const QString anchor(url.fragment());

    // A link with no path points inside the document which is already shown.
    if (path.isEmpty())
    {
        if (!this->scrollToDocumentAnchor(anchor))
        {
            RLogger::warning("Document \"%s\" contains no heading matching the link \"#%s\".\n",
                             this->currentFileName.toUtf8().constData(),
                             anchor.toUtf8().constData());
        }
        return;
    }

    // Everything else is another document of the same set, named relative to
    // the one which is shown.
    QString fileName(path);
    if (QFileInfo(path).isRelative())
    {
        const QString baseDirPath(this->currentFileName.isEmpty()
                                  ? QDir::currentPath()
                                  : QFileInfo(this->currentFileName).absolutePath());
        fileName = QDir::cleanPath(QDir(baseDirPath).filePath(path));
    }

    if (!QFileInfo::exists(fileName))
    {
        RLogger::warning("Link \"%s\" points to a document which does not exist - \"%s\".\n",
                         url.toString().toUtf8().constData(),
                         fileName.toUtf8().constData());
        return;
    }

    this->loadFile(fileName);
    this->selectListItem(fileName);

    if (!anchor.isEmpty())
    {
        this->scrollToDocumentAnchor(anchor);
    }
}
