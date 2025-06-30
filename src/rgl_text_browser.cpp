#include <QFontDatabase>

#include "rgl_text_browser.h"

RTextBrowser::RTextBrowser(bool hasClearButton, QWidget *parent)
  : QTextBrowser(parent)
  , hasClearButton(hasClearButton)
{
    this->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    if (this->hasClearButton)
    {
        QIcon clearIcon(":/icons/action/pixmaps/range-clear.svg");

        this->clearButton = new QPushButton(clearIcon,"Clear",this);

        QObject::connect(this->clearButton,
                         &QPushButton::clicked,
                         this,
                         &RTextBrowser::clear);
    }
}

void RTextBrowser::resizeEvent(QResizeEvent *event)
{
    QTextBrowser::resizeEvent(event);

    if (!this->hasClearButton)
    {
        return;
    }

    int bw = this->clearButton->width();
    int bh = this->clearButton->height();
    int tw = this->width();

    int x = tw - bw - 25;
    int y = 5;

    this->clearButton->setGeometry(x,y,bw,bh);
}
