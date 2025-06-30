#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>

#include "rgl_file_tags_dialog.h"

RFileTagsDialog::RFileTagsDialog(const RFileInfo &fileInfo, QWidget *parent)
    : QDialog{parent}
    , tags{fileInfo.getTags()}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-ok.svg");
    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");

    this->setWindowTitle(tr("Cloud file tags"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    if (!fileInfo.getId().isNull())
    {
        formLayout->addRow(tr("ID") + ":", new QLabel(fileInfo.getId().toString(QUuid::WithoutBraces)));
    }
    formLayout->addRow(tr("Name") + ":", new QLabel(fileInfo.getPath()));

    QGroupBox *tagsBox = new QGroupBox(tr("Tags") + ":");
    mainLayout->addWidget(tagsBox);

    QVBoxLayout *tagsLayout = new QVBoxLayout;
    tagsLayout->setSpacing(0);
    tagsLayout->setContentsMargins(0,0,0,0);
    tagsBox->setLayout(tagsLayout);

    QHBoxLayout *editLayout = new QHBoxLayout;
    editLayout->setSpacing(0);
    editLayout->setContentsMargins(0,0,0,0);
    tagsLayout->addLayout(editLayout);

    this->lineEdit = new QLineEdit;
    this->lineEdit->setPlaceholderText(tr("Add tag"));
    this->lineEdit->setClearButtonEnabled(false);
    this->lineEdit->setMinimumWidth(220);
    this->lineEdit->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    editLayout->addWidget(this->lineEdit);

    this->addButton = new QPushButton(addIcon,QString());
    this->addButton->setDisabled(true);
    this->addButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    editLayout->addWidget(this->addButton);

    this->labelLayout = new RFlowLayout;
    tagsLayout->addLayout(this->labelLayout);

    for (const QString &tag : fileInfo.getTags())
    {
        this->addLabel(tag);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(this->okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(this->lineEdit,&QLineEdit::textEdited,this,&RFileTagsDialog::onTextEdited);
    QObject::connect(this->addButton,&QPushButton::clicked,this,&RFileTagsDialog::onAddClicked);
}

QStringList RFileTagsDialog::getTags() const
{
    return this->tags;
}

void RFileTagsDialog::addLabel(const QString &text)
{
    RLabelWidget *personLabel = new RLabelWidget(text,QVariant(text),false);
    personLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    this->labelLayout->addWidget(personLabel);

    this->labelWidgets.insert(text,personLabel);

    QObject::connect(personLabel,&RLabelWidget::closed,this,&RFileTagsDialog::onCloseLabelClicked);
}

void RFileTagsDialog::onCloseLabelClicked(const QVariant &data)
{
    QString tag = data.toString();

    if (this->labelWidgets.contains(tag))
    {
        delete this->labelWidgets.find(tag).value();
    }

    this->tags.removeAll(tag);
    this->okButton->setEnabled(true);
}

void RFileTagsDialog::onTextEdited(const QString &text)
{
    this->addButton->setEnabled(RFileInfo::isTagValid(text) && !this->tags.contains(text) && this->tags.size() <= RFileInfo::MaxNumTags);
}

void RFileTagsDialog::onAddClicked()
{
    QString tag(this->lineEdit->text());
    if (!this->tags.contains(tag))
    {
        this->addLabel(tag);
        this->tags.append(tag);
        this->okButton->setEnabled(true);
    }
    this->lineEdit->clear();
}
