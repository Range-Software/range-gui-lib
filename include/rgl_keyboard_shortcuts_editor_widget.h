#ifndef RGL_KEYBOARD_SHORTCUTS_EDITOR_WIDGET_H
#define RGL_KEYBOARD_SHORTCUTS_EDITOR_WIDGET_H

#include <QTreeWidget>
#include <QLineEdit>

#include "rgl_action_list.h"
#include "rgl_keyboard_shortcut_line_edit.h"

class RKeyboardShortcutsEditorWidget : public QWidget
{

    Q_OBJECT

    protected:

        //! Tree view.
        QTreeWidget *tree;
        //! Line edit.
        RKeyboardShortcutLineEdit *lineEdit;
        //! Action list.
        const RActionList *actionList;

    public:

        //! Constructor.
        explicit RKeyboardShortcutsEditorWidget(const RActionList *actionList, QWidget *parent = nullptr);

        //! Reset to default.
        void resetToDefault();

    protected:

        //! Populate tree.
        void populateTree();

        //! Find items with given shortcut.
        QList<QTreeWidgetItem *> findItems(const QString &shortcut);

    signals:

        //! Shortcut changed signal.
        void shortcutChanged(uint position, const QString &shortcut);

    protected slots:

        //! Tree selection has changed.
        void onSelectionChanged();

        //! On reset all button clicked.
        void onResetAllClicked();

        //! Shortcut changed in shortcut line edit.
        void onShortcutChanged(const QString &shortcut);

};

#endif // RGL_KEYBOARD_SHORTCUTS_EDITOR_WIDGET_H
