#include <qmath.h>

#include <rbl_progress.h>

#include "rgl_progress_handler.h"

static void progressPrintHandler(double fraction);

static void progressInitializeHandler(const QString &message, bool pulseType);

static void progressFinalizeHandler(const QString &message);

void RProgressHandler::initialize()
{
    RProgress::getInstance().setProgressInitializeHandler(progressInitializeHandler);
    RProgress::getInstance().setProgressFinalizeHandler(progressFinalizeHandler);
    RProgress::getInstance().setProgressPrintHandler(progressPrintHandler);
}

RProgressHandler & RProgressHandler::getInstance()
{
    static RProgressHandler progress;
    return progress;
}

void RProgressHandler::print(double fraction)
{
    emit this->progress(fraction);
}

void RProgressHandler::emitProgressInitialize(const QString &message, bool pulseType)
{
    emit this->progressInitialize(message,pulseType);
}

void RProgressHandler::emitProgressFinalize(const QString &message)
{
    emit this->progressFinalize(message);
}

static void progressPrintHandler(double fraction)
{
    static double lastValue = 0.0;
    if (qAbs(qFloor(fraction*100) - lastValue) >= 1)
    {
        lastValue = qFloor(fraction*100);
        RProgressHandler::getInstance().print(fraction);
    }
}

static void progressInitializeHandler(const QString &message, bool pulseType)
{
    RProgressHandler::getInstance().emitProgressInitialize(message,pulseType);
}

static void progressFinalizeHandler(const QString &message)
{
    RProgressHandler::getInstance().emitProgressFinalize(message);
}
