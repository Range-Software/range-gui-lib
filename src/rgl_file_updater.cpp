#include <QFile>

#include <rbl_logger.h>
#include <rbl_progress.h>

#include "rgl_file_updater.h"

RFileUpdater::RFileUpdater()
{

}

void RFileUpdater::addFile(const QString &source, const QString &destination)
{
    this->files.append(QPair<QString,QString>(source,destination));
}

int RFileUpdater::perform()
{
    RLogger::info("Updating files.\n");
    RLogger::indent();
    RProgressInitialize("Updating files");
    for (int i=0;i<this->files.size();i++)
    {
        RProgressPrint(i+1,this->files.size());
        RLogger::info("Checking file \'%s\'.\n",this->files.at(i).first.toUtf8().constData());

        if (QFile::exists(this->files.at(i).second.toUtf8().constData()))
        {
            RLogger::info("Destination file exists \'%s\' and will not be updated.\n",this->files.at(i).second.toUtf8().constData());
        }
        else
        {
            RLogger::info("Copying file \'%s\' to \'%s\'\n",
                          this->files.at(i).first.toUtf8().constData(),
                          this->files.at(i).second.toUtf8().constData());
            if (!QFile::copy(this->files.at(i).first,this->files.at(i).second))
            {
                RLogger::warning("Failed to copy file \'%s\' to \'%s\'\n",
                                 this->files.at(i).first.toUtf8().constData(),
                                 this->files.at(i).second.toUtf8().constData());
            }
        }
    }
    RProgressFinalize();
    RLogger::unindent();

    return 0;
}
