#ifndef RGL_LOG_BROWSER_DIALOG_H
#define RGL_LOG_BROWSER_DIALOG_H

#include <QDialog>

#include "rgl_log_browser.h"

class RLogBrowserDialog : public QDialog
{

    Q_OBJECT

    protected:

        //! Log browser.
        RLogBrowser *logBrowser;

    public:

        //! Constructor.
        explicit RLogBrowserDialog(const QString &logFileName, QWidget *parent = nullptr);

    protected slots:

        //! Clear button was clicked.
        void onClearButtonClicked();

};

#endif // RGL_LOG_BROWSER_DIALOG_H
