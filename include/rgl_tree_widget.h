#ifndef RGL_TREE_WIDGET_H
#define RGL_TREE_WIDGET_H

#include <QTreeWidget>
#include <QMenu>
#include <QList>
#include <QString>

class RTreeWidgetToggleAction : public QAction
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RTreeWidgetToggleAction(int column, const QString &text, bool checked, QObject *parent)
            : QAction(text,parent)
        {
            this->setData(QVariant(column));
            this->setCheckable(true);
            this->setChecked(checked);

            QObject::connect(this,&QAction::toggled,this,&RTreeWidgetToggleAction::onToggled);
        }

    public slots:

        //! On action toggled.
        void onToggled(bool checked)
        {
            emit this->headerToggled(this->data().toInt(),checked);
        }

    signals:

        //! Action toggled
        void headerToggled(int column, bool checked);
};

class RTreeWidget : public QTreeWidget
{

    Q_OBJECT

    public:

        struct ColumnInfoItem
        {
            int column;
            QString title;
            bool hidden;
            bool available;
        };

    protected:

        QMenu *contextMenu;

    public:

        //! Constructor.
        explicit RTreeWidget(const QList<ColumnInfoItem> &columnInfo, QWidget *parent = nullptr);

    private slots:

        //! Costom menu requested.
        void onContextMenuRequested(const QPoint &pos);

        //! On action toggled.
        void onActionToggled(int column, bool checked);

};

#endif // RGL_TREE_WIDGET_H
