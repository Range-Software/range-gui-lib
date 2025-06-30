#ifndef RGL_LABEL_WIDGET_H
#define RGL_LABEL_WIDGET_H

#include <QWidget>
#include <QVariant>
#include <QPushButton>

class RLabelWidget : public QWidget
{
    Q_OBJECT

    protected:

        QVariant data;
        bool hideCloseButton;

        QPushButton *closeButton;

    public:

        explicit RLabelWidget(const QString &text, const QVariant &data, bool hideCloseButton, QWidget *parent = nullptr);

    protected:

        void enterEvent(QEnterEvent *);
        void leaveEvent(QEvent *);

    protected slots:

        void onCloseButtonClicked();

    signals:

        void closed(const QVariant &data);

};

#endif // RGL_LABEL_WIDGET_H
