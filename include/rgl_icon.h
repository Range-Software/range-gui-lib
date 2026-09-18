#ifndef RGL_ICON_H
#define RGL_ICON_H

#include <QIcon>
#include <QString>

class RIcon
{

    private:

        RIcon();

    public:

        //! Create icon from file.
        //! If dark variant exists (same file name in "dark" sub-directory) icon
        //! will switch between light and dark variant according to active palette.
        static QIcon fromFile(const QString &fileName);

        //! Return dark variant file name.
        static QString toDarkFileName(const QString &fileName);

        //! Return true if active application palette is dark.
        static bool isDarkPalette();

};

#endif // RGL_ICON_H
