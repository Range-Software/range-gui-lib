#ifndef RGL_DESKTOP_SERVICES_H
#define RGL_DESKTOP_SERVICES_H

#include <QString>

class RDesktopServices
{

    private:

        RDesktopServices();

    public:

        //! Open directory in the platform file browser (Finder, Explorer, Linux file manager).
        //! Return false if directory does not exist or could not be opened.
        static bool openDirectory(const QString &dirPath);

};

#endif // RGL_DESKTOP_SERVICES_H
