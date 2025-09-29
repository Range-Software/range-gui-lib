#ifndef RGL_CLOUD_REPORT_SENDER_H
#define RGL_CLOUD_REPORT_SENDER_H

#include <QObject>

#include <rcl_cloud_client.h>

#include "rgl_application_settings.h"

class RCloudReportSender : public QObject
{

    Q_OBJECT

    protected:

        //! Autodelete.
        bool autodelete;
        //! Cloud client.
        RCloudClient *cloudClient;

    public:

        //! Constructor.
        explicit RCloudReportSender(const RApplicationSettings *applicationSettings, QObject *parent = nullptr);

        //! Send report.
        void sendReport(const RReportRecord &reportRecord);

        //! Static method to send record.
        static void sendReport(const RApplicationSettings *applicationSettings, const RReportRecord &reportRecord);

    protected slots:

        //! Process is finished.
        void onSubmitReportFinished(QString message);

        //! Cloud action has failed.
        void onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

    signals:

        void reportSent(const QString &message);

        void reportFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

};

#endif // RGL_CLOUD_REPORT_SENDER_H
