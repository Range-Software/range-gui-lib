#ifndef RGL_HELP_ACTIONS_WIDGET_H
#define RGL_HELP_ACTIONS_WIDGET_H

#include "rgl_document_widget.h"
#include "rgl_action_list.h"

class RHelpActionsWidget : public RDocumentWidget
{

    Q_OBJECT

    public:

        //! Constructor
        explicit RHelpActionsWidget(const RActionList *actionList, const QString &searchPath, QWidget *parent = nullptr);

};

#endif // RGL_HELP_ACTIONS_WIDGET_H
