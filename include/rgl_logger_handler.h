#ifndef RGL_LOGGER_HANDLER_H
#define RGL_LOGGER_HANDLER_H

#include <QObject>

#include <rbl_message.h>

class RLoggerHandler : public QObject
{

    Q_OBJECT

    protected:

        //! Constructor.
        inline RLoggerHandler() { }

    public:

        //! Initialize RProgress static instance.
        static void initialize();

        //! Disable message sending to GUI.
        static void halt();

        //! Enable message sending to GUI.
        static void unhalt();

        //! Return reference to static object.
        static RLoggerHandler & getInstance();

        //! Log handler function.
        static void logHandler (const RMessage &message);

    signals:

        //! Signal containing info message.
        void infoPrinted(const QString &message);

        //! Signal containing notice message.
        void noticePrinted(const QString &message);

        //! Signal containing warning message.
        void warningPrinted(const QString &message);

        //! Signal containing error message.
        void errorPrinted(const QString &message);
};

#endif /* RGL_LOGGER_HANDLER_H */
