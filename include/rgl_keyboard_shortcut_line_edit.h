#ifndef RGL_KEYBOARD_SHORTCUT_LINE_EDIT_H
#define RGL_KEYBOARD_SHORTCUT_LINE_EDIT_H

#include <QLineEdit>

class RKeyboardShortcutLineEdit : public QLineEdit
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit RKeyboardShortcutLineEdit(QWidget *parent = nullptr);

    protected:

        //! Process key press event.
        void keyPressEvent(QKeyEvent *keyEvent);

    signals:

        //! Shortcut changed signal.
        void shortcutChanged(const QString &shortcut);

    protected slots:

        //! Process signal QLineEdit::textChanged()
        void onTextChanged(const QString &text);

};

#endif // RGL_KEYBOARD_SHORTCUT_LINE_EDIT_H
