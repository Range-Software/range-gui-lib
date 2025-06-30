#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>

#include "rgl_access_owner_dialog.h"

RAccessOwnerDialog::RAccessOwnerDialog(const QUuid &id, const QString &name, const RAccessOwner &accessOwner, QWidget *parent)
    : QDialog{parent}
    , accessOwner{accessOwner}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-ok.svg");

    this->setWindowTitle(tr("Access owner"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *accessModeLayout = new QFormLayout;
    mainLayout->addLayout(accessModeLayout);

    if (!id.isNull())
    {
        accessModeLayout->addRow(tr("ID") + ":", new QLabel(id.toString(QUuid::WithoutBraces)));
    }
    accessModeLayout->addRow(tr("Name") + ":", new QLabel(name));

    QLineEdit *userEdit = new QLineEdit(this->accessOwner.getUser());
    accessModeLayout->addRow(tr("User") + ":", userEdit);

    QLineEdit *groupEdit = new QLineEdit(this->accessOwner.getGroup());
    accessModeLayout->addRow(tr("Group") + ":", groupEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(this->okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(userEdit,&QLineEdit::textChanged,this,&RAccessOwnerDialog::onUserChanged);
    QObject::connect(groupEdit,&QLineEdit::textChanged,this,&RAccessOwnerDialog::onGroupChanged);
}

RAccessOwner RAccessOwnerDialog::getAccessOwner() const
{
    return this->accessOwner;
}

void RAccessOwnerDialog::onUserChanged(const QString &text)
{
    this->accessOwner.setUser(text);
    this->okButton->setEnabled(RAccessOwner::isUserValid(text));
}

void RAccessOwnerDialog::onGroupChanged(const QString &text)
{
    this->accessOwner.setGroup(text);
    this->okButton->setEnabled(RAccessOwner::isGroupValid(text));
}
