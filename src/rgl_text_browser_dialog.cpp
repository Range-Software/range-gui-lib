#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QDialogButtonBox>

#include "rgl_text_browser.h"
#include "rgl_text_browser_dialog.h"

RTextBrowserDialog::RTextBrowserDialog(const QString &title,
                                       const QString &desc,
                                       const QString &text,
                                       QWidget *parent)
    : QDialog(parent)
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->resize(700,500);

    this->setWindowTitle(title);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QLabel *titleLabel = new QLabel(desc);
    mainLayout->addWidget(titleLabel);

    RTextBrowser *textBrowser = new RTextBrowser(false,this);
    textBrowser->insertPlainText(text);
    mainLayout->addWidget(textBrowser);

    QTextCursor textCursor = textBrowser->textCursor();
    textCursor.movePosition(QTextCursor::Start);
    textBrowser->setTextCursor(textCursor);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);
}
