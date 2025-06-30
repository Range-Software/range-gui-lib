#include <QHBoxLayout>
#include <QLabel>

#include "rgl_label_widget.h"

RLabelWidget::RLabelWidget(const QString &text, const QVariant &data, bool hideCloseButton, QWidget *parent)
    : QWidget(parent)
    , data(data)
    , hideCloseButton(hideCloseButton)
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    QLabel *label = new QLabel(text);
    label->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    mainLayout->addWidget(label);

    this->closeButton = new QPushButton(closeIcon,QString());
    this->closeButton->setFixedSize(16,16);
    this->closeButton->setIconSize(QSize(8,8));
    mainLayout->addWidget(this->closeButton);
    if (this->hideCloseButton)
    {
        this->closeButton->hide();
    }

    QObject::connect(this->closeButton,&QPushButton::clicked,this,&RLabelWidget::onCloseButtonClicked);
}

void RLabelWidget::enterEvent(QEnterEvent *)
{
    if (this->hideCloseButton)
    {
        this->closeButton->show();
    }
}

void RLabelWidget::leaveEvent(QEvent *)
{
    if (this->hideCloseButton)
    {
        this->closeButton->hide();
    }
}

void RLabelWidget::onCloseButtonClicked()
{
    emit this->closed(this->data);
}
