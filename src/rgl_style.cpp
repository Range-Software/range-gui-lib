#include <QStyleFactory>
#include <QStyle>
#include <QPalette>
#include <QApplication>

#include <rbl_logger.h>

#include "rgl_application_settings.h"
#include "rgl_style.h"

RStyle::RStyle()
{

}

void RStyle::applyStyle(const QString &styleName)
{
    qobject_cast<QApplication*>(QCoreApplication::instance())->setStyleSheet(QLatin1String("QDialogButtonBox {"
                                                                             // "  button-layout: 0;"
                                                                             "  dialogbuttonbox-buttons-have-icons: 1;"
                                                                             "}"));

    RLogger::info("Applying style \'%s\'\n",styleName.toUtf8().constData());
    if (styleName == RApplicationSettings::Style::FusionRange)
    {
        QStyle *style = QStyleFactory::create("Fusion");
        QPalette palette(style->standardPalette());

        palette.setColor(QPalette::Highlight, QColor(150,0,0));

        QApplication::setPalette(palette);
        QApplication::setStyle(style);
    }
    else if (styleName == RApplicationSettings::Style::FusionDark)
    {
        QStyle *style = QStyleFactory::create("Fusion");
        QPalette palette(style->standardPalette());

        palette.setColor(QPalette::Window, QColor(61,67,69));
        palette.setColor(QPalette::WindowText, QColor(238,251,247));
        palette.setColor(QPalette::Base, QColor(35,39,41));
        palette.setColor(QPalette::AlternateBase, QColor(50,56,59));
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, QColor(238,251,247));
        palette.setColor(QPalette::Text, QColor(238,251,247));
        palette.setColor(QPalette::Button, QColor(35,39,41));
        palette.setColor(QPalette::ButtonText, QColor(238,251,247));
        palette.setColor(QPalette::BrightText, Qt::white);

        palette.setColor(QPalette::Light, QColor(73,73,73));
        palette.setColor(QPalette::Midlight, QColor(63,63,63));
        palette.setColor(QPalette::Dark, QColor(33,33,33));
        palette.setColor(QPalette::Mid, QColor(43,43,43));
        palette.setColor(QPalette::Shadow, QColor(0,0,0));

        palette.setColor(QPalette::Highlight, Qt::black);
        palette.setColor(QPalette::HighlightedText, QColor(238,251,247));


        palette.setColor(QPalette::Link, Qt::lightGray);
        palette.setColor(QPalette::LinkVisited, Qt::gray);

        palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);

        QApplication::setPalette(palette);
        QApplication::setStyle(style);
    }
    else if (styleName == RApplicationSettings::Style::WindowsDark)
    {
        QStyle *style = QStyleFactory::create("Windows");
        QPalette palette(style->standardPalette());

        palette.setColor(QPalette::Window, QColor(51,57,59));
        palette.setColor(QPalette::WindowText, QColor(238,251,247));
        palette.setColor(QPalette::Base, QColor(35,39,41));
        palette.setColor(QPalette::AlternateBase, QColor(50,56,59));
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, QColor(238,251,247));
        palette.setColor(QPalette::Text, QColor(238,251,247));
        palette.setColor(QPalette::Button, QColor(35,39,41));
        palette.setColor(QPalette::ButtonText, QColor(238,251,247));
        palette.setColor(QPalette::BrightText, Qt::white);

        palette.setColor(QPalette::Light, QColor(73,73,73));
        palette.setColor(QPalette::Midlight, QColor(63,63,63));
        palette.setColor(QPalette::Dark, QColor(33,33,33));
        palette.setColor(QPalette::Mid, QColor(43,43,43));
        palette.setColor(QPalette::Shadow, QColor(0,0,0));

        palette.setColor(QPalette::Highlight, Qt::black);
        palette.setColor(QPalette::HighlightedText, QColor(238,251,247));

        palette.setColor(QPalette::Link, Qt::lightGray);
        palette.setColor(QPalette::LinkVisited, Qt::gray);

        palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);

        QApplication::setPalette(palette);
        QApplication::setStyle(style);
    }
    else
    {
        QStyle *style = QStyleFactory::create(styleName);
        // QPalette palette(style->standardPalette());
        // QApplication::setPalette(palette);
        QApplication::setPalette(QPalette());
        QApplication::setStyle(style);
    }
}
