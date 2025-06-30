#include <QDir>

#include "rgl_help_actions_widget.h"

RHelpActionsWidget::RHelpActionsWidget(const RActionList *actionList, const QString &searchPath, QWidget *parent)
    : RDocumentWidget(searchPath,"action_index.html",parent)
{
    for (uint i=0;i<actionList->getNActions();i++)
    {
        const RAction *action = actionList->getAction(i);

        if (!action || action->isSeparator())
        {
            continue;
        }

        QIcon icon;
        if (action->icon().isNull())
        {
            icon = QIcon(":/icons/general/pixmaps/range-generic_action.svg");
        }
        else
        {
            icon = action->icon();
        }

        QDir docDir(searchPath);
        this->addListItem(icon, action->getGroupName() + " >> " + action->text(), docDir.filePath("action_" + action->getName() + ".html"));
    }
}
