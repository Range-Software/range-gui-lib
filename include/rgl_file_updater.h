#ifndef RGL_FILE_UPDATER_H
#define RGL_FILE_UPDATER_H

#include <QString>
#include <QList>
#include <QPair>

#include <rbl_job.h>

class RFileUpdater : public RJob
{

    Q_OBJECT

    protected:

        //! List of source/destination pairs.
        QList< QPair<QString,QString> > files;

    public:

        //! Constructor.
        explicit RFileUpdater();

        //! Add file to be updated.
        void addFile(const QString &source, const QString &destination);

    protected:

        //! Function performing actual job.
        virtual int perform() override final;

};

#endif // RGL_FILE_UPDATER_H
