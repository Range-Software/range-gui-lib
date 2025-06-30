#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QDir>

#include "rgl_help_dialog.h"
#include "rgl_help_document_widget.h"
#include "rgl_application.h"
#include "rgl_help_actions_widget.h"

RHelpDialog::RHelpDialog(const QString &title, const QMap<QString, QString> &topics, QWidget *parent)
    : QDialog{parent}
    , helpPath{RApplication::instance()->getApplicationSettings()->getHelpDir()}
    , languageCode{RApplication::instance()->getApplicationSettings()->getLanguageCode()}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(title);
    this->resize(1300,700);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->tabWidget = new QTabWidget;
    mainLayout->addWidget(this->tabWidget);

    for (auto iter = topics.cbegin(); iter != topics.cend(); ++iter)
    {
        RHelpDocumentWidget *helpDocumentWidget = new RHelpDocumentWidget(this->findHelpFilePath(iter.key()));
        this->tabWidget->addTab(helpDocumentWidget,iter.value());
    }

    RHelpActionsWidget *helpActionsWidget = new RHelpActionsWidget(RApplication::instance()->getActionList(),this->findHelpFilePath("actions"));
    this->tabWidget->addTab(helpActionsWidget,tr("Actions"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);
}

QString RHelpDialog::findHelpFilePath(const QString &topic) const
{
    return QDir(QDir(this->helpPath).filePath(topic)).filePath(this->languageCode);
}
