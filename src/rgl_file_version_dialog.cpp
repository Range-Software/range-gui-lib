#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QDialogButtonBox>

#include "rgl_file_version_dialog.h"

RFileVersionDialog::RFileVersionDialog(const RFileInfo &fileInfo, QWidget *parent)
    : QDialog{parent}
    , version{fileInfo.getVersion()}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-ok.svg");

    this->setWindowTitle(tr("Cloud file version"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *versionLayout = new QFormLayout;
    mainLayout->addLayout(versionLayout);

    if (!fileInfo.getId().isNull())
    {
        versionLayout->addRow(tr("ID") + ":", new QLabel(fileInfo.getId().toString(QUuid::WithoutBraces)));
    }
    versionLayout->addRow(tr("Name") + ":", new QLabel(fileInfo.getPath()));

    QSpinBox *majorVersionSpin = new QSpinBox;
    majorVersionSpin->setMinimum(0);
    majorVersionSpin->setValue(this->version.getMajor());
    versionLayout->addRow(tr("Major") + ":",majorVersionSpin);

    QSpinBox *minorVersionSpin = new QSpinBox;
    minorVersionSpin->setMinimum(0);
    minorVersionSpin->setValue(this->version.getMinor());
    versionLayout->addRow(tr("Minor") + ":",minorVersionSpin);

    QSpinBox *releaseVersionSpin = new QSpinBox;
    releaseVersionSpin->setMinimum(0);
    releaseVersionSpin->setValue(this->version.getRelease());
    versionLayout->addRow(tr("Release") + ":",releaseVersionSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(this->okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(majorVersionSpin,&QSpinBox::valueChanged,this,&RFileVersionDialog::onMajorValueChanged);
    QObject::connect(minorVersionSpin,&QSpinBox::valueChanged,this,&RFileVersionDialog::onMinorValueChanged);
    QObject::connect(releaseVersionSpin,&QSpinBox::valueChanged,this,&RFileVersionDialog::onReleaseValueChanged);
}

RVersion RFileVersionDialog::getVersion() const
{
    return this->version;
}

void RFileVersionDialog::onMajorValueChanged(int value)
{
    this->version = RVersion(value,
                             this->version.getMinor(),
                             this->version.getRelease());
    this->okButton->setEnabled(true);
}

void RFileVersionDialog::onMinorValueChanged(int value)
{
    this->version = RVersion(this->version.getMajor(),
                             value,
                             this->version.getRelease());
    this->okButton->setEnabled(true);
}

void RFileVersionDialog::onReleaseValueChanged(int value)
{
    this->version = RVersion(this->version.getMajor(),
                             this->version.getMinor(),
                             value);
    this->okButton->setEnabled(true);
}
