#include <rbl_locker.h>

#include "rgl_locker_handler.h"

void RLockerHandler::initialize(void)
{
    RLocker::getInstance().setLockHandler(RLockerHandler::handler);
    RLocker::getInstance().setLockHandler(RLockerHandler::handler);
}

RLockerHandler & RLockerHandler::getInstance(void)
{
    static RLockerHandler locker;
    return locker;
}

QMutex * RLockerHandler::getMutexPtr(void)
{
    return &this->mutex;
}

void RLockerHandler::handler(bool lock)
{
    if (lock)
    {
        RLockerHandler::getInstance().getMutexPtr()->lock();
    }
    else
    {
        RLockerHandler::getInstance().getMutexPtr()->unlock();
    }
}
