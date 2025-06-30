#include <rbl_logger.h>

#include "rgl_logger_handler.h"

void RLoggerHandler::initialize()
{
    RLogger::getInstance().setLevel(R_LOG_LEVEL_DETAIL);
    RLogger::getInstance().setHalted(true);
    RLogger::getInstance().setLogHandler(RLoggerHandler::logHandler);
}

void RLoggerHandler::halt()
{
    RLogger::getInstance().setHalted(true);
}

void RLoggerHandler::unhalt()
{
    RLogger::getInstance().setHalted(false);
}

RLoggerHandler & RLoggerHandler::getInstance()
{
    static RLoggerHandler logger;
    return logger;
}

void RLoggerHandler::logHandler(const RMessage &message)
{
    switch (message.getType())
    {
        case RMessage::Type::Info:
            emit RLoggerHandler::getInstance().infoPrinted(message);
            break;
        case RMessage::Type::Notice:
            emit RLoggerHandler::getInstance().noticePrinted(message);
            break;
        case RMessage::Type::Warning:
            emit RLoggerHandler::getInstance().warningPrinted(message);
            break;
        case RMessage::Type::Error:
            emit RLoggerHandler::getInstance().errorPrinted(message);
            break;
        case RMessage::Type::Debug:
            emit RLoggerHandler::getInstance().infoPrinted(message);
            break;
        case RMessage::Type::Trace:
            emit RLoggerHandler::getInstance().infoPrinted(message);
            break;
        default:
            emit RLoggerHandler::getInstance().errorPrinted("UNKNOWN MESSAGE TYPE: " + message);
            break;
    }
}
