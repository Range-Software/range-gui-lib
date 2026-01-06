#include <QStyleFactory>
#include <QStyle>
#include <QPalette>
#include <QApplication>
#include <QStyleHints>

#include <rbl_logger.h>

#include "rgl_style.h"

RStyle::RStyle()
{

}

void RStyle::applyStyle(const QString &styleName, Qt::ColorScheme colorScheme)
{
    qobject_cast<QApplication*>(QCoreApplication::instance())->setStyleSheet(QLatin1String("QDialogButtonBox {"
                                                                             // "  button-layout: 0;"
                                                                             "  dialogbuttonbox-buttons-have-icons: 1;"
                                                                             "}"));

    if (QGuiApplication::styleHints()->colorScheme() != colorScheme)
    {
        QGuiApplication::styleHints()->setColorScheme(colorScheme);
    }

    RLogger::info("Applying style \'%s\' with color scheme \'%s\'\n",
                  styleName.toUtf8().constData(),
                  RStyle::colorSchemeToText(colorScheme).toUtf8().constData());

    QStyle *style = QStyleFactory::create(styleName);
    // QPalette palette(style->standardPalette());
    // QApplication::setPalette(palette);
    QApplication::setPalette(QPalette());
    QApplication::setStyle(style);
}

QString RStyle::colorSchemeToText(Qt::ColorScheme colorScheme)
{
    if (colorScheme == Qt::ColorScheme::Light)
    {
        return QCoreApplication::translate("RStyle","Light");
    }
    else if (colorScheme == Qt::ColorScheme::Dark)
    {
        return QCoreApplication::translate("RStyle","Dark");
    }
    else
    {
        return QCoreApplication::translate("RStyle","System");
    }
}
