#include <QHeaderView>
#include <QAction>

#include "rgl_tree_widget.h"

RTreeWidget::RTreeWidget(const QList<ColumnInfoItem> &columnInfo, QWidget *parent)
    : QTreeWidget{parent}
{
    this->setColumnCount(columnInfo.count());
    this->setSortingEnabled(true);
    this->setSelectionMode(QAbstractItemView::SingleSelection);

    QTreeWidgetItem *headerItem = new QTreeWidgetItem();

    this->contextMenu = new QMenu(this);

    for (const ColumnInfoItem &item : columnInfo)
    {
        headerItem->setText(item.column,item.title);
        this->setColumnHidden(item.column,item.hidden);

        if (item.available)
        {
            RTreeWidgetToggleAction *action = new RTreeWidgetToggleAction(item.column,item.title,!item.hidden,this);
            this->contextMenu->addAction(action);
            QObject::connect(action,&RTreeWidgetToggleAction::headerToggled,this,&RTreeWidget::onActionToggled);
        }
    }

    this->setHeaderItem(headerItem);
    this->header()->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(this->header(), &QTreeWidget::customContextMenuRequested, this, &RTreeWidget::onContextMenuRequested);
}

void RTreeWidget::onContextMenuRequested(const QPoint &pos)
{
    this->contextMenu->popup(this->header()->mapToGlobal(pos));
}

void RTreeWidget::onActionToggled(int column, bool checked)
{
    this->setColumnHidden(column,!checked);
}
