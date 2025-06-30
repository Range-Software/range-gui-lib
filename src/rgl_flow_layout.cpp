#include <QWidget>

#include "rgl_flow_layout.h"

RFlowLayout::RFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent)
    , m_hSpace(hSpacing)
    , m_vSpace(vSpacing)
{
    this->setContentsMargins(margin, margin, margin, margin);
}

RFlowLayout::RFlowLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing)
    , m_vSpace(vSpacing)
{
    this->setContentsMargins(margin, margin, margin, margin);
}

RFlowLayout::~RFlowLayout()
{
    QLayoutItem *item;
    while ((item = this->takeAt(0)))
    {
        delete item;
    }
}

void RFlowLayout::addItem(QLayoutItem *item)
{
    this->itemList.append(item);
}

int RFlowLayout::horizontalSpacing() const
{
    if (this->m_hSpace >= 0)
    {
        return this->m_hSpace;
    }
    else
    {
        return this->smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int RFlowLayout::verticalSpacing() const
{
    if (this->m_vSpace >= 0)
    {
        return this->m_vSpace;
    }
    else
    {
        return this->smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int RFlowLayout::count() const
{
    return this->itemList.size();
}

QLayoutItem *RFlowLayout::itemAt(int index) const
{
    return this->itemList.value(index);
}

QLayoutItem *RFlowLayout::takeAt(int index)
{
    if (index >= 0 && index < this->itemList.size())
    {
        return this->itemList.takeAt(index);
    }
    return nullptr;
}

Qt::Orientations RFlowLayout::expandingDirections() const
{
    return { };
}

bool RFlowLayout::hasHeightForWidth() const
{
    return true;
}

int RFlowLayout::heightForWidth(int width) const
{
    int height = this->doLayout(QRect(0, 0, width, 0), true);
    return height;
}

void RFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    this->doLayout(rect, false);
}

QSize RFlowLayout::sizeHint() const
{
    return this->minimumSize();
}

QSize RFlowLayout::minimumSize() const
{
    QSize size;
    for (const QLayoutItem *item : std::as_const(this->itemList))
    {
        size = size.expandedTo(item->minimumSize());
    }

    const QMargins margins = this->contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

int RFlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    this->getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (QLayoutItem *item : std::as_const(this->itemList))
    {
        const QWidget *wid = item->widget();
        int spaceX = this->horizontalSpacing();
        if (spaceX == -1)
        {
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        }
        int spaceY = this->verticalSpacing();
        if (spaceY == -1)
        {
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        }

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0)
        {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
        {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int RFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent)
    {
        return -1;
    }
    else if (parent->isWidgetType())
    {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    }
    else
    {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
