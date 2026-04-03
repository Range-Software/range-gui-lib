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
        this->clearButton->setAutoDefault(false);
        this->clearButton->setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(128, 128, 128, 60);"
            "   border: none;"
            "   border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(128, 128, 128, 180);"
            "}"
        );

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
