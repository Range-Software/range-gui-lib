#include <QProcess>
#include <QFileInfo>

#include <rbl_error.h>
#include <rbl_logger.h>

#include "rgl_open_ssl_tool.h"

const QString ROpenSslTool::CertificateSubject::Country::key = "C";
const QString ROpenSslTool::CertificateSubject::Country::name = "Country";
const QString ROpenSslTool::CertificateSubject::State::key = "ST";
const QString ROpenSslTool::CertificateSubject::State::name = "State";
const QString ROpenSslTool::CertificateSubject::Location::key = "L";
const QString ROpenSslTool::CertificateSubject::Location::name = "Location";
const QString ROpenSslTool::CertificateSubject::Organization::key = "O";
const QString ROpenSslTool::CertificateSubject::Organization::name = "Organization";
const QString ROpenSslTool::CertificateSubject::OrganizationUnit::key = "OU";
const QString ROpenSslTool::CertificateSubject::OrganizationUnit::name = "Organization unit";
const QString ROpenSslTool::CertificateSubject::CommonName::key = "CN";
const QString ROpenSslTool::CertificateSubject::CommonName::name = "Common name";

ROpenSslTool::ROpenSslTool(const ROpenSslToolSettings &openSslToolSettings, QObject *parent)
    : QObject{parent}
    , openSslToolSettings{openSslToolSettings}
{}

void ROpenSslTool::generateKey(const QString &keyPath, const QString &keyPassword) const
{
    QProcess process;

    QStringList opensslArgs =
    {
        "genpkey",
        "-algorithm", "RSA",
        "-pkeyopt", QString("rsa_keygen_bits:%1").arg(4096),
        "-out", keyPath,
        "-pass", QString("pass:%1").arg(keyPassword)
    };

    RLogger::info("OpenSslTool: Excuting: GenKEY: %s %s\n",
                  openSslToolSettings.getOpenSslPath().toUtf8().constData(),
                  opensslArgs.join(' ').toUtf8().constData());

    process.start(openSslToolSettings.getOpenSslPath(),opensslArgs);
    if (!process.waitForStarted())
    {
        QByteArray processError = process.readAllStandardError();
        throw RError(RError::Application,R_ERROR_REF,"Failed to start \'openssl: generate csr\' process. %s",processError.constData());
    }
    if (!process.waitForFinished())
    {
        QByteArray processError = process.readAllStandardError();
        throw RError(RError::Application,R_ERROR_REF,"Failed to finish \'openssl: generate csr\' process. %s",processError.constData());
    }
    QByteArray processOutput = process.readAllStandardOutput();
    if (!processOutput.isEmpty())
    {
        RLogger::info("OpenSslTool: %s",processOutput.constData());
    }
}

void ROpenSslTool::generateCsr(const QString &keyPath, const QString &keyPassword, const QMap<QString, QString> &subjectMap, const QString &csrPath) const
{
    if (!QFileInfo::exists(this->openSslToolSettings.getOpenSslCnfPath()))
    {
        throw RError(RError::Application,R_ERROR_REF,"OpenSSL configuration file \'%s\' does not exist.",this->openSslToolSettings.getOpenSslCnfPath().toUtf8().constData());
    }

    QStringList subjectFlags =
    {
        CertificateSubject::Country::key,
        CertificateSubject::State::key,
        CertificateSubject::Location::key,
        CertificateSubject::Organization::key,
        CertificateSubject::OrganizationUnit::key,
        CertificateSubject::CommonName::key
    };

    QString subjectString;

    for (const auto &subjectFlag : subjectFlags)
    {
        if (subjectMap.contains(subjectFlag))
        {
            subjectString += QString("/%1=%2").arg(subjectFlag,subjectMap.value(subjectFlag));
        }
    }

    QProcess process;

    QStringList opensslArgs =
    {
        "req",
        "-key", keyPath,
        "-passin", QString("pass:%1").arg(keyPassword),
        "-new",
        "-sha256",
        "-subj", subjectString,
        "-config", this->openSslToolSettings.getOpenSslCnfPath(),
        "-out", csrPath,
        "-batch",
        "-verbose"
    };

    RLogger::info("Excuting: GenCSR: %s %s\n",
                  openSslToolSettings.getOpenSslPath().toUtf8().constData(),
                  opensslArgs.join(' ').toUtf8().constData());

    process.start(openSslToolSettings.getOpenSslPath(),opensslArgs);
    if (!process.waitForStarted())
    {
        throw RError(RError::Application,R_ERROR_REF,"Failed to start \'openssl: generate csr\' process.");
    }
    if (!process.waitForFinished())
    {
        throw RError(RError::Application,R_ERROR_REF,"Failed to finish \'openssl: generate csr\' process.");
    }
    QByteArray processOutput = process.readAllStandardOutput();
    RLogger::info("%s",processOutput.constData());
}

void ROpenSslTool::outputKeyAsText(const QString &keyPath, const QString &keyPassword, const QString &textPath) const
{
    QProcess process;

    QStringList opensslArgs =
        {
            "pkey",
            "-in", keyPath,
            "-passin", QString("pass:%1").arg(keyPassword),
            "-out", textPath,
            "-text",
            "-noout"
        };

    RLogger::info("OpenSslTool: Excuting: OutKEY: %s %s\n",
                  openSslToolSettings.getOpenSslPath().toUtf8().constData(),
                  opensslArgs.join(' ').toUtf8().constData());

    process.start(openSslToolSettings.getOpenSslPath(),opensslArgs);
    if (!process.waitForStarted())
    {
        QByteArray processError = process.readAllStandardError();
        throw RError(RError::Application,R_ERROR_REF,"Failed to start \'openssl: output key\' process. %s",processError.constData());
    }
    if (!process.waitForFinished())
    {
        QByteArray processError = process.readAllStandardError();
        throw RError(RError::Application,R_ERROR_REF,"Failed to finish \'openssl: output key\' process. %s",processError.constData());
    }
    QByteArray processOutput = process.readAllStandardOutput();
    if (!processOutput.isEmpty())
    {
        RLogger::info("OpenSslTool: %s",processOutput.constData());
    }
}

void ROpenSslTool::outputCertificateAsText(const QString &certPath, const QString &textPath) const
{
    QProcess process;

    QStringList opensslArgs =
        {
            "x509",
            "-in", certPath,
            "-out", textPath,
            "-text"
        };

    RLogger::info("OpenSslTool: Excuting: OutCert: %s %s\n",
                  openSslToolSettings.getOpenSslPath().toUtf8().constData(),
                  opensslArgs.join(' ').toUtf8().constData());

    process.start(openSslToolSettings.getOpenSslPath(),opensslArgs);
    if (!process.waitForStarted())
    {
        QByteArray processError = process.readAllStandardError();
        throw RError(RError::Application,R_ERROR_REF,"Failed to start \'openssl: output certificate\' process. %s",processError.constData());
    }
    if (!process.waitForFinished())
    {
        QByteArray processError = process.readAllStandardError();
        throw RError(RError::Application,R_ERROR_REF,"Failed to finish \'openssl: output certificate\' process. %s",processError.constData());
    }
    QByteArray processOutput = process.readAllStandardOutput();
    if (!processOutput.isEmpty())
    {
        RLogger::info("OpenSslTool: %s",processOutput.constData());
    }
}
