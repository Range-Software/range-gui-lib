#include <rbl_logger.h>

#include "rgl_ai_consent.h"
#include "rgl_application_settings.h"
#include "rgl_message_box.h"

bool RAiConsent::request(QWidget *parent,
                         RApplicationSettings *applicationSettings,
                         const QString &serviceId,
                         const QString &serviceName,
                         const QString &sharedData)
{
    if (applicationSettings->getAiDataSharingConsent(serviceId))
    {
        return true;
    }

    const QString text = "<p>" + tr("To answer your question, the following data will be sent to <b>%1</b>:").arg(serviceName.toHtmlEscaped()) + "</p>"
                       + "<p>" + sharedData + "</p>"
                       + "<p>" + tr("This data may include personal information about you and the people in your family tree. It is processed under the privacy policy and terms of that service.") + "</p>"
                       + "<p>" + tr("Do you agree to send this data?") + "</p>";

    if (RMessageBox::question(parent,tr("Share data with AI service?"),text) != RMessageBox::Yes)
    {
        RLogger::info("Sharing of data with AI service \'%s\' was declined.\n",serviceId.toUtf8().constData());
        return false;
    }

    RLogger::info("Sharing of data with AI service \'%s\' was allowed.\n",serviceId.toUtf8().constData());
    applicationSettings->setAiDataSharingConsent(serviceId,true);
    return true;
}
