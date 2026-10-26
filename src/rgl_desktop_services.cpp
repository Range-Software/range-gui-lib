#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QUrl>

#include <rbl_logger.h>

#include "rgl_desktop_services.h"

bool RDesktopServices::openDirectory(const QString &dirPath)
{
    QFileInfo dirInfo(dirPath);
    if (!dirInfo.isDir())
    {
        RLogger::error("Directory \'%s\' does not exist\n",dirPath.toUtf8().constData());
        return false;
    }

    // QUrl::fromLocalFile() produces a valid file URL on every platform, including
    // Windows drive letters and paths containing spaces or non-ASCII characters.
    QUrl url(QUrl::fromLocalFile(QDir::cleanPath(dirInfo.absoluteFilePath())));
    if (!QDesktopServices::openUrl(url))
    {
        RLogger::error("Failed to open directory \'%s\'\n",url.toLocalFile().toUtf8().constData());
        return false;
    }
    return true;
}
