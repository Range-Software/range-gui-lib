#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QIconEngine>
#include <QPainter>
#include <QPalette>

#include "rgl_icon.h"

namespace
{

//! Icon engine delegating to light or dark icon depending on palette active at paint time.
class RThemedIconEngine : public QIconEngine
{

    protected:

        QIcon lightIcon;
        QIcon darkIcon;

    public:

        RThemedIconEngine(const QString &lightFileName, const QString &darkFileName)
            : lightIcon(lightFileName)
            , darkIcon(darkFileName)
        {

        }

        void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override
        {
            this->getIcon().paint(painter,rect,Qt::AlignCenter,mode,state);
        }

        QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override
        {
            return this->getIcon().pixmap(size,mode,state);
        }

        QPixmap scaledPixmap(const QSize &size, QIcon::Mode mode, QIcon::State state, qreal scale) override
        {
            return this->getIcon().pixmap(size,scale,mode,state);
        }

        QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override
        {
            return this->getIcon().actualSize(size,mode,state);
        }

        QList<QSize> availableSizes(QIcon::Mode mode, QIcon::State state) override
        {
            return this->getIcon().availableSizes(mode,state);
        }

        bool isNull() override
        {
            return this->lightIcon.isNull();
        }

        QString key() const override
        {
            return QStringLiteral("RThemedIconEngine");
        }

        QIconEngine *clone() const override
        {
            return new RThemedIconEngine(*this);
        }

    protected:

        const QIcon &getIcon() const
        {
            return RIcon::isDarkPalette() ? this->darkIcon : this->lightIcon;
        }

};

} // namespace

RIcon::RIcon()
{

}

QIcon RIcon::fromFile(const QString &fileName)
{
    QString darkFileName(RIcon::toDarkFileName(fileName));
    if (!QFile::exists(darkFileName))
    {
        return QIcon(fileName);
    }
    return QIcon(new RThemedIconEngine(fileName,darkFileName));
}

QString RIcon::toDarkFileName(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    return fileInfo.path() + QLatin1String("/dark/") + fileInfo.fileName();
}

bool RIcon::isDarkPalette()
{
    const QPalette palette = QGuiApplication::palette();
    return palette.color(QPalette::Window).lightness() < palette.color(QPalette::WindowText).lightness();
}
