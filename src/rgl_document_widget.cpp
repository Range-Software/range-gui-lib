#include <QSplitter>
#include <QVBoxLayout>
#include <QFile>
#include <QDir>

#include <rbl_logger.h>

#include "rgl_document_widget.h"
#include "rgl_message_box.h"

RDocumentWidget::RDocumentWidget(const QString &searchPath, const QString &defaultFileName, QWidget *parent)
    : QWidget{parent}
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
    this->listWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    splitter->addWidget(this->listWidget);

    QObject::connect(this->listWidget,&QListWidget::itemSelectionChanged,this,&RDocumentWidget::onListSelectionChanged);

    this->textBrowser = new QTextBrowser;
    this->textBrowser->setSearchPaths(QStringList() << searchPath);
    this->textBrowser->setReadOnly(true);
    this->textBrowser->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
    splitter->addWidget(this->textBrowser);

    splitter->setStretchFactor(1,1);

    this->loadFile(this->defaultFileName);
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
        }
    }
}

void RDocumentWidget::onListSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = this->listWidget->selectedItems();

    if (selectedItems.size() == 0)
    {
        this->loadFile(this->defaultFileName);
    }
    else
    {
        this->loadFile(selectedItems.at(0)->data(Qt::UserRole).toString());
    }
}
