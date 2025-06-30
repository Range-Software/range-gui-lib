#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "rgl_document_dialog.h"
#include "rgl_document_widget.h"

RDocumentDialog::RDocumentDialog(const QString &title, QMap<QString, QString> helpPaths, QWidget *parent)
    : QDialog{parent}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(title);
    this->resize(1300,700);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->tabWidget = new QTabWidget;
    mainLayout->addWidget(this->tabWidget);

    for (auto it = helpPaths.constBegin();it != helpPaths.constEnd(); ++it)
    {
        this->tabWidget->addTab(new RDocumentWidget(it.value()),it.key());
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);
}
