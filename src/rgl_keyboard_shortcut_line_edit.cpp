#include <QKeyEvent>

#include "rgl_keyboard_shortcut_line_edit.h"

RKeyboardShortcutLineEdit::RKeyboardShortcutLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    this->setClearButtonEnabled(true);

    QObject::connect(this,
                     &QLineEdit::textChanged,
                     this,
                     &RKeyboardShortcutLineEdit::onTextChanged);
}

void RKeyboardShortcutLineEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    QString grab;
    bool emitSignal = false;

    int modifiers = keyEvent->modifiers();

    if (modifiers & Qt::ControlModifier)
    {
        grab.append("Ctrl+");
    }
    if (modifiers & Qt::ShiftModifier)
    {
        grab.append("Shift+");
    }
    if (modifiers & Qt::AltModifier)
    {
        grab.append("Alt+");
    }
    if (QChar(keyEvent->key()).isLetter())
    {
        grab.append(QChar(keyEvent->key()).toUpper());
        emitSignal = true;
    }
    this->setText(grab);

    if (emitSignal)
    {
        emit this->shortcutChanged(this->text());
    }
}

void RKeyboardShortcutLineEdit::onTextChanged(const QString &text)
{
    if (text.isEmpty())
    {
        emit this->shortcutChanged(this->text());
    }
}
