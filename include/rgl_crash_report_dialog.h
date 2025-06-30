#ifndef RGL_CRASH_REPORT_DIALOG_H
#define RGL_CRASH_REPORT_DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>

#include <rcl_cloud_client.h>

#include "rgl_application_settings.h"
#include "rgl_log_browser.h"

class RCrashReportDialog : public QDialog
{

    Q_OBJECT

    protected:

        //! Cloud client.
        RCloudClient *cloudClient;

        //! Log browser.
        RLogBrowser *logBrowser;

        //! Text edit;
        QPlainTextEdit *textEdit;

    public:

        //! Constructor.
        explicit RCrashReportDialog(RApplicationSettings *applicationSettings,
                                    const QString &logFile,
                                    QWidget *parent = nullptr);

    protected slots:

        //! Report button clicked.
        void onReportClicked();

        //! Process is finished.
        void onSubmitReportFinished(QString message);

        //! Cloud action has finished.
        void onCloudActionFinished();

        //! Cloud action has failed.
        void onCloudActionFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

};

#endif // RGL_CRASH_REPORT_DIALOG_H
