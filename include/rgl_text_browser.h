#ifndef RGL_TEXT_BROWSER_H
#define RGL_TEXT_BROWSER_H

#include <QTextBrowser>
#include <QPushButton>
#include <QList>

class RTextBrowser : public QTextBrowser
{

    Q_OBJECT

    protected:

        //! Clear button enabled.
        bool hasClearButton;
        //! Clear button.
        QPushButton *clearButton;
        //! Widgets placed in the top right corner, left of the clear button.
        QList<QWidget*> cornerWidgets;

    public:

        //! Constructor.
        explicit RTextBrowser(bool hasClearButton, QWidget *parent = nullptr);

        //! Add widget to the top right corner, left of the clear button and of previously added widgets.
        //! Browser takes ownership of the widget and styles it as the clear button.
        void addCornerWidget(QWidget *widget);

    protected:

        //! Place clear button and corner widgets to the top right corner.
        void updateCornerWidgetsGeometry();

        //! Change event.
        virtual void changeEvent(QEvent *event) override;

        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event) override;
        
};

#endif // RGL_TEXT_BROWSER_H
