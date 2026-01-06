#ifndef RGL_STYLE_H
#define RGL_STYLE_H

#include <QString>

class RStyle
{

    private:

        RStyle();

    public:

        static void applyStyle(const QString &styleName, Qt::ColorScheme colorScheme = Qt::ColorScheme::Unknown);

        static QString colorSchemeToText(Qt::ColorScheme colorScheme);

};

#endif // RGL_STYLE_H
