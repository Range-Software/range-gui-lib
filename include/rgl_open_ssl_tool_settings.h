#ifndef RGL_OPEN_SSL_TOOL_SETTINGS_H
#define RGL_OPEN_SSL_TOOL_SETTINGS_H

#include <QString>

class ROpenSslToolSettings
{

protected:

    //! Internal initialization function.
    void _init(const ROpenSslToolSettings *pOpenSslToolSettings = nullptr);

protected:

    //! Path to OpenSSL tool.
    QString openSslPath;
    //! Path to OpenSSL configuration.
    QString openSslCnfPath;
    //! Path to log directory.
    QString logDirectory;
    //! Path to working directory.
    QString workingDirectory;

public:

    //! Constructor.
    ROpenSslToolSettings();

    //! Copy constructor.
    ROpenSslToolSettings(const ROpenSslToolSettings &openSslToolSettings);

    //! Destructor.
    ~ROpenSslToolSettings();

    //! Assignment operator.
    ROpenSslToolSettings &operator =(const ROpenSslToolSettings &openSslToolSettings);

    //! Return const reference to OpenSSL tool path.
    QString getOpenSslPath() const;

    //! Set new OpenSSL tool path.
    void setOpenSslPath(const QString &openSslPath);

    //! Return const reference to OpenSSL config path.
    QString getOpenSslCnfPath() const;

    //! Set new OpenSSL config path.
    void setOpenSslCnfPath(const QString &openSslCnfPath);

    //! Return const reference to log directory path.
    const QString &getLogDirectory() const;

    //! Set new log directory path.
    void setLogDirectory(const QString &logDirectory);

    //! get const reference to working directory path.
    const QString &getWorkingDirectory() const;

    //! Set new working directory path.
    void setWorkingDirectory(const QString &workingDirectory);

};

#endif // RGL_OPEN_SSL_TOOL_SETTINGS_H
