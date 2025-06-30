#ifndef RGL_PROGRESS_HANDLER_H
#define RGL_PROGRESS_HANDLER_H

#include <QObject>

class RProgressHandler : public QObject
{

    Q_OBJECT

    protected:

        //! Constructor.
        inline RProgressHandler() { }

    public:

        //! Initialize RProgress static instance.
        static void initialize();

        //! Return reference to static object.
        static RProgressHandler & getInstance();

        //! Emit progress signal.
        void print(double fraction);

        //! Emit initialize signal.
        void emitProgressInitialize(const QString &message, bool pulseType);

        //! Emit finalize signal.
        void emitProgressFinalize(const QString &message);

    signals:

        //! Signal containing progress fraction.
        void progress(double fraction);

        //! Initialize signal.
        void progressInitialize(const QString &message, bool pulseType);

        //! Finalize signal.
        void progressFinalize(const QString &message);
};

#endif /* RGL_PROGRESS_HANDLER_H */
