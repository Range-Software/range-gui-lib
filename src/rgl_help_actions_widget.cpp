#include <QDir>

#include "rgl_help_actions_widget.h"

RHelpActionsWidget::RHelpActionsWidget(const RActionList *actionList, const QString &searchPath, QWidget *parent)
    : RDocumentWidget(searchPath,"action_index.md",parent)
{
    QDir docDir(searchPath);

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

        QString htmlIndexFile = docDir.filePath("action_" + action->getName() + ".html");
        QString mdIndexFile = docDir.filePath("action_" + action->getName() + ".md");

        if (QFileInfo::exists(mdIndexFile))
        {
            this->addListItem(icon, action->getGroupName() + " >> " + action->text(), mdIndexFile);
        }
        else if (QFileInfo::exists(htmlIndexFile))
        {
            this->addListItem(icon, action->getGroupName() + " >> " + action->text(), htmlIndexFile);
        }
    }
}
