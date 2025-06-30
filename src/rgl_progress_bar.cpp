#include "rgl_progress_bar.h"

RProgressBar::RProgressBar(QWidget *parent)
    : QProgressBar(parent)
    , autoHide(true)
{
    this->setTextVisible(true);
    this->pulseTimer = new QTimer(this);
    this->pulseTimer->setInterval(100);
    QObject::connect(this->pulseTimer,&QTimer::timeout,this,&RProgressBar::pulse);
}

void RProgressBar::startPulse(void)
{
    this->setRange(0,0);
    this->QProgressBar::setValue(0);
    this->pulseTimer->start();
}

void RProgressBar::stopPulse(void)
{
    if (this->pulseTimer->isActive())
    {
        this->pulseTimer->stop();
    }
}

void RProgressBar::setValue(int value)
{
    this->setFormat(this->message + " (" + QString::number(value) + " %)");
    this->QProgressBar::setValue(value);
}

void RProgressBar::setMessage(const QString &message)
{
    this->message = message;
    this->setFormat(this->message);
}

void RProgressBar::setAutoHide(bool autoHide)
{
    this->autoHide = autoHide;
    if (this->autoHide && this->value() == 100)
    {
        this->hide();
    }
    else
    {
        this->show();
    }
}

void RProgressBar::hide(void)
{
    if (this->autoHide)
    {
        this->QProgressBar::hide();
    }
}

void RProgressBar::pulse(void)
{
    this->QProgressBar::setValue(this->value() + 2);
}
