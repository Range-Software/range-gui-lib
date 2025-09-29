#ifndef RGL_CRASH_REPORT_DIALOG_H
#define RGL_CRASH_REPORT_DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>

#include "rgl_application_settings.h"
#include "rgl_cloud_report_sender.h"
#include "rgl_log_browser.h"

class RCrashReportDialog : public QDialog
{

    Q_OBJECT

    protected:

        //! Report sender.
        RCloudReportSender *reportSender;
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
        void onReportSent(QString message);

        //! Cloud action has failed.
        void onReportFailed(RError::Type errorType, const QString &errorMessage, const QString &message);

};

#endif // RGL_CRASH_REPORT_DIALOG_H
