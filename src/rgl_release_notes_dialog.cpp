#include "rgl_release_notes_dialog.h"
#include "rgl_application.h"
#include "rgl_help_document_widget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDir>

RReleaseNotesDialog::RReleaseNotesDialog(QWidget *parent)
    : QDialog{parent}
{
    QString releaseNotesPath = RApplication::instance()->getApplicationSettings()->findDocDir();

    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Release notes"));
    this->resize(1300,700);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    RHelpDocumentWidget *helpDocumentWidget = new RHelpDocumentWidget(releaseNotesPath,"RELEASE_NOTES.json");
    mainLayout->addWidget(helpDocumentWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);
}
