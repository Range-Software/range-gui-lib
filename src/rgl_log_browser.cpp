#include <QFileSystemWatcher>
#include <QFile>
#include <QTextStream>

#include <rbl_logger.h>

#include "rgl_log_browser.h"

RLogBrowser::RLogBrowser(const QString &logFileName, QWidget *parent) :
    RTextBrowser(false,parent),
    logFileName(logFileName),
    fileStreamEnd(0)
{
    QFileSystemWatcher *systemWatcher = new QFileSystemWatcher(QStringList(this->logFileName),this);

    this->readFile(this->logFileName,true);

    QObject::connect(systemWatcher,&QFileSystemWatcher::fileChanged,this,&RLogBrowser::onFileChanged);
}

void RLogBrowser::clearLog()
{
    QFile file(this->logFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        RLogger::warning("Failed to open the file \'%s\' for writing.\n", this->logFileName.toUtf8().constData());
        return;
    }
    file.close();

    this->clear();
}

void RLogBrowser::readFile(const QString &path, bool whole)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        RLogger::warning("Failed to open the file \'%s\' for reading.\n", path.toUtf8().constData());
        return;
    }

    QString fileContent;

    try
    {
        qint64 fileSize = file.size();

        if (fileSize <= this->fileStreamEnd)
        {
            this->fileStreamEnd = 0;
            this->clear();
        }

        QTextStream fileStream(&file);

        fileStream.seek(this->fileStreamEnd);

        while (!fileStream.atEnd())
        {
            if (whole)
            {
                if (!fileContent.isEmpty())
                {
                    fileContent.append('\n');
                }
                fileContent.append(fileStream.readLine());
            }
            else
            {
                this->append(fileStream.readLine());
            }
        }
        this->fileStreamEnd = fileStream.pos();
        if (whole)
        {
            this->setText(fileContent);
        }
    }
    catch (...)
    {
        RLogger::error("Unknown error while reading the log file.\n");
    }

    file.close();
}

void RLogBrowser::onFileChanged(const QString &path)
{
    this->readFile(path,false);
}
