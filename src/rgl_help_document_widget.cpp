#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <rbl_error.h>
#include <rbl_logger.h>

#include "rgl_help_document_widget.h"

RHelpDocumentWidget::RHelpDocumentWidget(const QString &searchPath, const QString &indexFile, QWidget *parent)
    : RDocumentWidget{searchPath,QString(),parent}
{
    QString indexFilePath = searchPath + QDir::separator() + indexFile;
    try
    {
        this->readIndexFile(indexFilePath,searchPath);
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to read index file \"%s\". %s\n",indexFilePath.toUtf8().constData(),error.getMessage().toUtf8().constData());
    }
}

void RHelpDocumentWidget::readIndexFile(const QString &fileName, const QString &searchPath)
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \"%s\".",fileName.toUtf8().constData());
    }

    RLogger::info("Reading JSON file \"%s\"\n",fileName.toUtf8().constData());
    RLogger::indent();

    QByteArray byteArray = file.readAll();
    RLogger::info("Successfuly read \"%ld\" bytes from \"%s\".\n",byteArray.size(),file.fileName().toUtf8().constData());

    const QJsonObject &json = QJsonDocument::fromJson(byteArray).object();

    if (const QJsonValue v = json["main"]; v.isString())
    {
        QString baseName(v.toString());
        if (!baseName.isEmpty())
        {
            this->defaultFileName = searchPath + QDir::separator() + v.toString();
        }
    }

    if (const QJsonValue v = json["chapters"]; v.isArray())
    {
        const QJsonArray &jsonArray = v.toArray();

        for (const QJsonValue &jsonValue : jsonArray)
        {
            if (jsonValue.isObject())
            {
                const QJsonObject &jsonChapter = jsonValue.toObject();

                QString title;
                if (const QJsonValue &vTitle = jsonChapter["title"]; vTitle.isString())
                {
                    title = vTitle.toString();
                }
                QString file;
                if (const QJsonValue &vFile = jsonChapter["file"]; vFile.isString())
                {
                    file = vFile.toString();
                }

                if (!title.isEmpty() && !file.isEmpty())
                {
                    this->addListItem(title,searchPath + QDir::separator() + file);
                }
            }
        }
    }

    file.close();

    RLogger::unindent();

    this->loadFile(this->defaultFileName);
}
