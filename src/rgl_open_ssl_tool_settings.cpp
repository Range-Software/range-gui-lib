#include "rgl_open_ssl_tool_settings.h"

void ROpenSslToolSettings::_init(const ROpenSslToolSettings *pOpenSslToolSettings)
{
    if (pOpenSslToolSettings)
    {
        this->openSslPath = pOpenSslToolSettings->openSslPath;
        this->openSslCnfPath = pOpenSslToolSettings->openSslCnfPath;
        this->logDirectory = pOpenSslToolSettings->logDirectory;
        this->workingDirectory = pOpenSslToolSettings->workingDirectory;
    }
}

ROpenSslToolSettings::ROpenSslToolSettings()
{
    this->_init();
}

ROpenSslToolSettings::ROpenSslToolSettings(const ROpenSslToolSettings &openSslToolSettings)
{
    this->_init(&openSslToolSettings);
}

ROpenSslToolSettings::~ROpenSslToolSettings()
{

}

ROpenSslToolSettings &ROpenSslToolSettings::operator =(const ROpenSslToolSettings &openSslToolSettings)
{
    this->_init(&openSslToolSettings);
    return (*this);
}

QString ROpenSslToolSettings::getOpenSslPath() const
{
    return this->openSslPath;
}

void ROpenSslToolSettings::setOpenSslPath(const QString &openSslPath)
{
    this->openSslPath = openSslPath;
}

QString ROpenSslToolSettings::getOpenSslCnfPath() const
{
    return this->openSslCnfPath;
}

void ROpenSslToolSettings::setOpenSslCnfPath(const QString &openSslCnfPath)
{
    this->openSslCnfPath = openSslCnfPath;
}

const QString &ROpenSslToolSettings::getLogDirectory() const
{
    return this->logDirectory;
}

void ROpenSslToolSettings::setLogDirectory(const QString &logDirectory)
{
    this->logDirectory = logDirectory;
}

const QString &ROpenSslToolSettings::getWorkingDirectory() const
{
    return this->workingDirectory;
}

void ROpenSslToolSettings::setWorkingDirectory(const QString &workingDirectory)
{
    this->workingDirectory = workingDirectory;
}
