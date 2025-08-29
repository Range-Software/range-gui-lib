#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>

//#define _USE_QDIALOG_EXEC

#ifndef _USE_QDIALOG_EXEC
#include <QWindow>
#endif

#include "rgl_message_box.h"
#include "rgl_text_browser.h"

RMessageBox::RMessageBox(QWidget *parent,
                         const QIcon &icon,
                         const QString &title,
                         const QString &text,
                         QDialogButtonBox::StandardButtons buttons,
                         RError::Type errorType,
                         const QString &errorMessage)
    : QDialog{parent}
{
    this->setWindowTitle(title);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    this->setLayout(mainLayout);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    mainLayout->addLayout(hBoxLayout);

    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(icon.pixmap(64));
    hBoxLayout->addWidget(iconLabel);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    hBoxLayout->addLayout(vBoxLayout);

    if (errorType != RError::None)
    {
        QFormLayout *formLayout = new QFormLayout;
        vBoxLayout->addLayout(formLayout);

        formLayout->addRow(tr("Error code") + ":", new QLabel(QString::number(errorType)));
        formLayout->addRow(tr("Error message") + ":", new QLabel(errorMessage));

        RTextBrowser *textBrowser = new RTextBrowser(false);
        textBrowser->setPlainText(text);
        vBoxLayout->addWidget(textBrowser);
    }
    else
    {
        QLabel *textLabel = new QLabel(text);
        textLabel->setWordWrap(true);
        vBoxLayout->addWidget(textLabel);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(buttons);
    mainLayout->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


int RMessageBox::RunSync(QDialog *dialog)
{
#ifdef _USE_QDIALOG_EXEC
    return dialog->exec();
#else
    QWidget *parent = nullptr;
    if (dialog->parent() && dialog->parent()->isWidgetType()) {
        parent = static_cast<QWidget*>(dialog->parent()); // safe after the check
    }
    if (parent)
    {
        dialog->setParent(parent);
    }
    dialog->setWindowModality(Qt::WindowModal);
    dialog->setAttribute(Qt::WA_DeleteOnClose, false);   // important for stack-allocated dialogs

    // Ensure transient parenting is fully established on Wayland
    if (parent)
    {
        parent->winId();
    }
    // dialog->winId();
    if (dialog->windowHandle() && parent && parent->windowHandle())
    {
        dialog->windowHandle()->setTransientParent(parent->windowHandle());
    }

    QEventLoop loop;
    int code = QDialog::Rejected;

    QObject::connect(dialog, &QDialog::finished, &loop, [&](int r){ code = r; loop.quit(); });
    QObject::connect(dialog, &QObject::destroyed,       &loop, [&]{ code = QDialog::Rejected; loop.quit(); });

    dialog->open(); // non-blocking show; Wayland config/placement happens here
    loop.exec();    // blocks this function until finished/destroyed is emitted
    return code;    // same codes as exec(): QDialog::Accepted/Rejected
#endif
}

RMessageBox::StandardButton RMessageBox::critical(QWidget *parent,
                                                  const QString &title,
                                                  const QString &text,
                                                  RError::Type errorType,
                                                  const QString &errorMessage)
{
    RMessageBox dialog(parent,QIcon::fromTheme(QIcon::ThemeIcon::DialogError),title,text,QDialogButtonBox::Ok,errorType,errorMessage);
    RMessageBox::RunSync(&dialog);
    return RMessageBox::Ok;
}

RMessageBox::StandardButton RMessageBox::information(QWidget *parent, const QString &title, const QString &text)
{
    RMessageBox dialog(parent,QIcon::fromTheme(QIcon::ThemeIcon::DialogInformation),title,text,QDialogButtonBox::Ok);
    RMessageBox::RunSync(&dialog);
    return RMessageBox::Ok;
}

RMessageBox::StandardButton RMessageBox::question(QWidget *parent, const QString &title, const QString &text)
{
    RMessageBox dialog(parent,QIcon::fromTheme(QIcon::ThemeIcon::DialogQuestion),title,text,QDialogButtonBox::Yes|QDialogButtonBox::No);
    int response = RMessageBox::RunSync(&dialog);
    return (response == QDialog::Accepted) ? RMessageBox::Yes : RMessageBox::No;
}

RMessageBox::StandardButton RMessageBox::warning(QWidget *parent, const QString &title, const QString &text)
{
    RMessageBox dialog(parent,QIcon::fromTheme(QIcon::ThemeIcon::DialogWarning),title,text,QDialogButtonBox::Ok);
    RMessageBox::RunSync(&dialog);
    return RMessageBox::Ok;
}

RMessageBox::StandardButton RMessageBox::quit(QWidget *parent, const QString &text)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon quitIcon(":/icons/application/pixmaps/range-quit.svg");

    QDialog dialog(parent);
    dialog.setWindowTitle(tr("Quit?"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    dialog.setLayout(mainLayout);

    QHBoxLayout *messageLayout = new QHBoxLayout;
    mainLayout->addLayout(messageLayout);

    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit).pixmap(64));
    messageLayout->addWidget(iconLabel);

    QLabel *textLabel = new QLabel(text + tr("Are you sure you want to quit?"));
    textLabel->setWordWrap(true);
    messageLayout->addWidget(textLabel);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *quitButton = new QPushButton(quitIcon, tr("Quit"));
    buttonBox->addButton(quitButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    int response = RMessageBox::RunSync(&dialog);
    return (response == QDialog::Accepted) ? RMessageBox::Yes : RMessageBox::No;
}
