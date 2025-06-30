#include <QContextMenuEvent>
#include <QMenu>
#include <QDialog>
#include <QVBoxLayout>

#include "rgl_html_viewer.h"

RHtmlViewer::RHtmlViewer(QWidget *parent)
    : QTextBrowser{parent}
{
}

void RHtmlViewer::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    QAction *showSourceAction = menu->addAction(tr("Show source"));
    QObject::connect(showSourceAction,&QAction::triggered,this,&RHtmlViewer::onShowSource);
    menu->exec(event->globalPos());
    delete menu;
}

void RHtmlViewer::onShowSource()
{
    QDialog *textDialog = new QDialog(this);

    QVBoxLayout *dialogLayout = new QVBoxLayout;
    textDialog->setLayout(dialogLayout);

    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setPlainText(this->toHtml());
    dialogLayout->addWidget(textBrowser);

    textDialog->exec();
}
