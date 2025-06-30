#ifndef RGL_HELP_DOCUMENT_WIDGET_H
#define RGL_HELP_DOCUMENT_WIDGET_H

#include <QMap>
#include <QString>

#include "rgl_document_widget.h"

class RHelpDocumentWidget : public RDocumentWidget
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RHelpDocumentWidget(const QString &searchPath, const QString &indexFile = "index.json", QWidget *parent = nullptr);

    private:

        void readIndexFile(const QString &fileName, const QString &searchPath);


};

#endif // RGL_HELP_DOCUMENT_WIDGET_H
