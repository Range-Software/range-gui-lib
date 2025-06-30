#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

//#include <rbl_logger.h>

#include "rgl_file_chooser_button.h"

RFileChooserButton::RFileChooserButton(const QString &labelText,
                                       Type type,
                                       const QString &caption,
                                       const QString &path,
                                       const QString &filter,
                                       QWidget *parent)
    : QWidget(parent)
    , type(type)
    , caption(caption)
    , path(path)
    , filter(filter)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);

    QIcon::ThemeIcon iconName = QIcon::ThemeIcon::DocumentOpen;
    if (this->type == RFileChooserButton::SaveFile)
    {
        iconName = QIcon::ThemeIcon::DocumentSave;
    }
    else if (this->type == RFileChooserButton::OpenFile)
    {
        iconName = QIcon::ThemeIcon::DocumentOpen;
    }
    else if (this->type == RFileChooserButton::Directory)
    {
        iconName = QIcon::ThemeIcon::FolderOpen;
    }

    QIcon iconClear(":/icons/action/pixmaps/range-clear.svg");

    if (!labelText.isEmpty())
    {
        QLabel *label = new QLabel(labelText);
        label->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
        layout->addWidget(label);
    }

    this->pathButton = new QPushButton(QIcon::fromTheme(iconName),QString());
    this->pathButton->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
    layout->addWidget(this->pathButton);

    this->clearButton = new QPushButton(iconClear,"");
    layout->addWidget(this->clearButton);

    this->setFileName(this->path);

    QObject::connect(this->pathButton,&QPushButton::clicked,this,&RFileChooserButton::onPathButtonClicked);
    QObject::connect(this->clearButton,&QPushButton::clicked,this,&RFileChooserButton::onClearButtonClicked);
}

const QString &RFileChooserButton::getSearchDirectory() const
{
    return this->searchDirectory;
}

void RFileChooserButton::setSearchDirectory(const QString &searchDirectory)
{
    this->searchDirectory = searchDirectory;
}

void RFileChooserButton::hideClearButton()
{
    this->clearButton->hide();
}

QString RFileChooserButton::getFileName() const
{
    return this->path;
}

void RFileChooserButton::setFileName(const QString &fileName)
{
    this->path = fileName;
    QString pushButtonText;
    if (this->path.isEmpty())
    {
        pushButtonText += "(" + tr("empty") + ")";
    }
    else
    {
        QFileInfo fileInfo(this->path);

        pushButtonText = fileInfo.fileName();

        if (this->type != RFileChooserButton::SaveFile && !fileInfo.exists())
        {
            pushButtonText += " (" + tr("does not exist") + ")";
        }
    }
    this->pathButton->setText(pushButtonText);

    this->clearButton->setDisabled(this->path.isEmpty());

    emit this->fileNameChanged(this->path);
}

void RFileChooserButton::setFilter(const QString &filter)
{
    this->filter = filter;
}

void RFileChooserButton::onPathButtonClicked()
{
    QString fileName;
    QString searchPath = this->path;

    if (this->type != RFileChooserButton::SaveFile)
    {
        QFileInfo fileInfo(searchPath);
        if (!fileInfo.exists())
        {
            if (QFileInfo::exists(fileInfo.path()))
            {
                searchPath = fileInfo.path();
            }
            else
            {
                searchPath = this->searchDirectory;
            }
        }
    }

    switch (this->type)
    {
        case RFileChooserButton::SaveFile:
        {
            fileName = QFileDialog::getSaveFileName(this,
                                                    this->caption,
                                                    this->path,
                                                    this->filter);
            break;
        }
        case RFileChooserButton::OpenFile:
        {
            fileName = QFileDialog::getOpenFileName(this,
                                                    this->caption,
                                                    searchPath,
                                                    this->filter);
            if (!fileName.isEmpty())
            {
                if (!QFile::exists(fileName))
                {
                    fileName.clear();
                }
            }
            break;
        }
        case RFileChooserButton::Directory:
        {
            fileName = QFileDialog::getExistingDirectory(this,
                                                         this->caption,
                                                         searchPath);
            if (!fileName.isEmpty())
            {
                if (!QFile::exists(fileName))
                {
                    fileName.clear();
                }
            }
            break;
        }
        default:
        {
//            RLogger::error("Unknown RFileChooserButton::type\n");
            break;
        }
    }

    if (!fileName.isEmpty())
    {
        this->setFileName(fileName);
    }
}

void RFileChooserButton::onClearButtonClicked()
{
    this->setFileName("");
}
