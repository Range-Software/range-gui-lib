#include <QAbstractSpinBox>
#include <QEvent>
#include <QFontDatabase>

#include "rgl_text_browser.h"
#include "rgl_icon.h"

//! Return themed icon file name.
static QString themedIconFile(const QString &fileName)
{
    return RIcon::isDarkPalette() ? RIcon::toDarkFileName(fileName) : fileName;
}

//! Style sheet of the corner widgets (clear button and added widgets), spin box arrows are drawn as icons.
static QString cornerWidgetStyleSheet(const QWidget *widget)
{
    QString className(widget->metaObject()->className());

    QString styleSheet = className + " {"
                         "   background-color: rgba(128, 128, 128, 60);"
                         "   border: none;"
                         "   border-radius: 4px;"
                         "}"
                       + className + ":hover {"
                         "   background-color: rgba(128, 128, 128, 180);"
                         "}";

    if (qobject_cast<const QAbstractSpinBox*>(widget))
    {
        styleSheet += className + "::up-button, " + className + "::down-button {"
                      "   background: transparent;"
                      "   border: none;"
                      "   width: 14px;"
                      "}"
                    + className + "::up-arrow {"
                      "   image: url(" + themedIconFile(":/icons/action/pixmaps/range-spin_up.svg") + ");"
                      "   width: 10px;"
                      "   height: 10px;"
                      "}"
                    + className + "::down-arrow {"
                      "   image: url(" + themedIconFile(":/icons/action/pixmaps/range-spin_down.svg") + ");"
                      "   width: 10px;"
                      "   height: 10px;"
                      "}";
    }

    return styleSheet;
}

RTextBrowser::RTextBrowser(bool hasClearButton, QWidget *parent)
  : QTextBrowser(parent)
  , hasClearButton(hasClearButton)
{
    this->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    if (this->hasClearButton)
    {
        QIcon clearIcon = RIcon::fromFile(":/icons/action/pixmaps/range-clear.svg");

        this->clearButton = new QPushButton(clearIcon,tr("Clear"),this);
        this->clearButton->setAutoDefault(false);
        this->clearButton->setStyleSheet(cornerWidgetStyleSheet(this->clearButton));

        QObject::connect(this->clearButton,
                         &QPushButton::clicked,
                         this,
                         &RTextBrowser::clear);
    }
}

void RTextBrowser::addCornerWidget(QWidget *widget)
{
    widget->setParent(this);
    widget->setStyleSheet(cornerWidgetStyleSheet(widget));
    this->cornerWidgets.append(widget);
    this->updateCornerWidgetsGeometry();
}

void RTextBrowser::updateCornerWidgetsGeometry()
{
    int x = this->width() - 25;
    int y = 5;

    if (this->hasClearButton)
    {
        int bw = this->clearButton->width();
        int bh = this->clearButton->height();

        x -= bw;
        this->clearButton->setGeometry(x,y,bw,bh);
        x -= 5;
    }

    for (QWidget *widget : std::as_const(this->cornerWidgets))
    {
        QSize size = widget->sizeHint();
        int height = this->hasClearButton ? this->clearButton->height() : size.height();

        x -= size.width();
        widget->setGeometry(x,y,size.width(),height);
        x -= 5;
    }
}

void RTextBrowser::changeEvent(QEvent *event)
{
    QTextBrowser::changeEvent(event);

    if (event->type() == QEvent::PaletteChange)
    {
        // Spin box arrow icons follow the color scheme.
        for (QWidget *widget : std::as_const(this->cornerWidgets))
        {
            widget->setStyleSheet(cornerWidgetStyleSheet(widget));
        }
    }
}

void RTextBrowser::resizeEvent(QResizeEvent *event)
{
    QTextBrowser::resizeEvent(event);

    this->updateCornerWidgetsGeometry();
}
