#ifndef RGL_LOG_BROWSER_H
#define RGL_LOG_BROWSER_H

#include "rgl_text_browser.h"

class RLogBrowser : public RTextBrowser
{

    Q_OBJECT

    protected:

        //! Log file name.
        const QString logFileName;
        //! End stream position.
        qint64 fileStreamEnd;

    public:

        //! Constructor.
        explicit RLogBrowser(const QString &logFileName, QWidget *parent = nullptr);

        //! Clear log.
        void clearLog();

    private:

        //! Read file
        void readFile(const QString &path, bool whole);

    private slots:

        //! File changed signal.
        void onFileChanged(const QString &path);

};

#endif // RGL_LOG_BROWSER_H
