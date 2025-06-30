#ifndef RGL_LOCKER_HANDLER_H
#define RGL_LOCKER_HANDLER_H

#include <QMutex>

class RLockerHandler
{

    protected:

        //! Constructor.
        inline RLockerHandler() { }

        //! Mutex.
        QMutex mutex;

    public:

        //! Initialize RLocker static instance.
        static void initialize(void);

        //! Return reference to static object.
        static RLockerHandler & getInstance(void);

        //! Return pointer to mutex.
        QMutex * getMutexPtr(void);

        static void handler(bool lock);
};

#endif /* RGL_LOCKER_HANDLER_H */
