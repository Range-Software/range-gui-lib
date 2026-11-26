#ifndef RGL_AI_CONSENT_H
#define RGL_AI_CONSENT_H

#include <QCoreApplication>
#include <QString>

class QWidget;
class RApplicationSettings;

//! Asks the user for permission before personal data is shared with an AI service.
//! The answer is remembered per service, so the question is asked only once.
class RAiConsent
{

    Q_DECLARE_TR_FUNCTIONS(RAiConsent)

    public:

        //! Return true if data may be sent to the service identified by serviceId.
        //! serviceName is shown to the user, sharedData describes what will be sent.
        static bool request(QWidget *parent,
                            RApplicationSettings *applicationSettings,
                            const QString &serviceId,
                            const QString &serviceName,
                            const QString &sharedData);

};

#endif // RGL_AI_CONSENT_H
