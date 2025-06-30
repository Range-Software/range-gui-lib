#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>

#include "rgl_access_mode_dialog.h"

RAccessModeDialog::RAccessModeDialog(const QUuid &id, const QString &name, const RAccessMode &accessMode, RAccessMode::ModeMask filter, QWidget *parent)
    : QDialog{parent}
    , accessMode{accessMode}
{
    this->validAccesses = RAccessModeDialog::accessModeMaskToCloudAccessList(filter);

    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-ok.svg");

    this->setWindowTitle(tr("Access mode"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *accessModeLayout = new QFormLayout;
    mainLayout->addLayout(accessModeLayout);

    if (!id.isNull())
    {
        accessModeLayout->addRow(tr("ID") + ":", new QLabel(id.toString(QUuid::WithoutBraces)));
    }
    accessModeLayout->addRow(tr("Name") + ":", new QLabel(name));

    QComboBox *userCombo = new QComboBox;
    for (CloudAccess validAccess : std::as_const(this->validAccesses))
    {
        userCombo->addItem(RAccessModeDialog::getModeName(validAccess),validAccess);
    }
    userCombo->setCurrentIndex(this->findAccessComboIndex(RAccessMode::filterModeMask(this->accessMode.getUserModeMask(),filter)));
    accessModeLayout->addRow(tr("User") + ":",userCombo);

    QComboBox *groupCombo = new QComboBox;
    for (CloudAccess validAccess : std::as_const(this->validAccesses))
    {
        groupCombo->addItem(RAccessModeDialog::getModeName(validAccess),validAccess);
    }
    groupCombo->setCurrentIndex(this->findAccessComboIndex(RAccessMode::filterModeMask(this->accessMode.getGroupModeMask(),filter)));
    accessModeLayout->addRow(tr("Group") + ":",groupCombo);

    QComboBox *otherCombo = new QComboBox;
    for (CloudAccess validAccess : std::as_const(this->validAccesses))
    {
        otherCombo->addItem(RAccessModeDialog::getModeName(validAccess),validAccess);
    }
    otherCombo->setCurrentIndex(this->findAccessComboIndex(RAccessMode::filterModeMask(this->accessMode.getOtherModeMask(),filter)));
    accessModeLayout->addRow(tr("Other") + ":",otherCombo);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(this->okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(userCombo,&QComboBox::currentIndexChanged,this,&RAccessModeDialog::onUserComboIndexChanged);
    QObject::connect(groupCombo,&QComboBox::currentIndexChanged,this,&RAccessModeDialog::onGroupComboIndexChanged);
    QObject::connect(otherCombo,&QComboBox::currentIndexChanged,this,&RAccessModeDialog::onOtherComboIndexChanged);
}

RAccessMode RAccessModeDialog::getAccessMode() const
{
    return this->accessMode;
}

qsizetype RAccessModeDialog::findAccessComboIndex(RAccessMode::ModeMask modeMask) const
{
    CloudAccess access = RAccessModeDialog::accessModeMaskToCloudAccess(modeMask);
    return RAccessModeDialog::findPositionInCloudAccessList(access,this->validAccesses);
}

QString RAccessModeDialog::getModeName(CloudAccess cloudAccess)
{
    switch (cloudAccess)
    {
        case None:             return tr("None");
        case Read:             return tr("Read");
        case Write:            return tr("Write");
        case Execute:          return tr("Execute");
        case ReadWrite:        return tr("Read & Write");
        case ReadExecute:      return tr("Read & Execute");
        case WriteExecute:     return tr("Write & Execute");
        case ReadWriteExecute: return tr("Read & Write & Execute");
        default:               return QString();
    }
}

RAccessMode::ModeMask RAccessModeDialog::getModeMask(CloudAccess cloudAccess)
{
    switch (cloudAccess)
    {
        case None:             return RAccessMode::None;
        case Read:             return RAccessMode::R;
        case Write:            return RAccessMode::W;
        case Execute:          return RAccessMode::X;
        case ReadWrite:        return RAccessMode::RW;
        case ReadExecute:      return RAccessMode::RX;
        case WriteExecute:     return RAccessMode::WX;
        case ReadWriteExecute: return RAccessMode::RWX;
        default:               return RAccessMode::None;
    }
}

RAccessModeDialog::CloudAccess RAccessModeDialog::accessModeMaskToCloudAccess(RAccessMode::ModeMask modeMask)
{
    if (modeMask == RAccessMode::None)     return None;
    else if (modeMask == RAccessMode::R)   return Read;
    else if (modeMask == RAccessMode::W)   return Write;
    else if (modeMask == RAccessMode::X)   return Execute;
    else if (modeMask == RAccessMode::RW)  return ReadWrite;
    else if (modeMask == RAccessMode::RX)  return ReadExecute;
    else if (modeMask == RAccessMode::WX)  return WriteExecute;
    else if (modeMask == RAccessMode::RWX) return ReadWriteExecute;
    return None;
}

QList<RAccessModeDialog::CloudAccess> RAccessModeDialog::accessModeMaskToCloudAccessList(RAccessMode::ModeMask modeMask)
{
    QList<RAccessMode::Mode> modeList = RAccessMode::modeMaskToModes(modeMask);
    QList<RAccessModeDialog::CloudAccess> accessList;

    accessList.append(RAccessModeDialog::None);
    for (qsizetype i=0;i<modeList.size();i++)
    {
        accessList.append(RAccessModeDialog::accessModeMaskToCloudAccess(modeList[i]));
        for (qsizetype j=i+1;j<modeList.size();j++)
        {
            accessList.append(RAccessModeDialog::accessModeMaskToCloudAccess(modeList[i] | modeList[j]));
        }
    }

    std::sort(accessList.begin(), accessList.end());

    return accessList;
}

qsizetype RAccessModeDialog::findPositionInCloudAccessList(CloudAccess access, const QList<CloudAccess> &accessList)
{
    for (qsizetype i=0;i<accessList.size();i++)
    {
        if (access == accessList[i])
        {
            return i;
        }
    }
    return 0;
}

void RAccessModeDialog::onUserComboIndexChanged(int index)
{
    this->accessMode.setUserModeMask(RAccessModeDialog::getModeMask(this->validAccesses[index]));
    this->okButton->setEnabled(true);
}

void RAccessModeDialog::onGroupComboIndexChanged(int index)
{
    this->accessMode.setGroupModeMask(RAccessModeDialog::getModeMask(this->validAccesses[index]));
    this->okButton->setEnabled(true);
}

void RAccessModeDialog::onOtherComboIndexChanged(int index)
{
    this->accessMode.setOtherModeMask(RAccessModeDialog::getModeMask(this->validAccesses[index]));
    this->okButton->setEnabled(true);
}
