#ifndef RGL_OPEN_SSL_TOOL_H
#define RGL_OPEN_SSL_TOOL_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QByteArray>

#include "rgl_open_ssl_tool_settings.h"

class ROpenSslTool : public QObject
{

    Q_OBJECT

    public:

        struct CertificateSubject
        {
            struct Country
            {
                static const QString key;
                static const QString name;
            };
            struct State
            {
                static const QString key;
                static const QString name;
            };
            struct Location
            {
                static const QString key;
                static const QString name;
            };
            struct Organization
            {
                static const QString key;
                static const QString name;
            };
            struct OrganizationUnit
            {
                static const QString key;
                static const QString name;
            };
            struct CommonName
            {
                static const QString key;
                static const QString name;
            };
        };

    protected:

        //! Path to OpenSSL tool.
        ROpenSslToolSettings openSslToolSettings;

    public:

        //! Constructor.
        explicit ROpenSslTool(const ROpenSslToolSettings &openSslToolSettings, QObject *parent = nullptr);

        //! Generate key.
        void generateKey(const QString &keyPath, const QString &keyPassword) const;

        //! Generate CSR.
        void generateCsr(const QString &keyPath, const QString &keyPassword, const QMap<QString,QString> &subjectMap, const QString &csrPath) const;

        //! Output key.
        void outputKeyAsText(const QString &keyPath, const QString &keyPassword, const QString &textPath) const;

        //! Output certificate.
        void outputCertificateAsText(const QString &certPath, const QString &textPath) const;

    signals:

};

#endif // RGL_OPEN_SSL_TOOL_H
