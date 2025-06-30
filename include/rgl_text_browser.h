#ifndef RGL_TEXT_BROWSER_H
#define RGL_TEXT_BROWSER_H

#include <QTextBrowser>
#include <QPushButton>

class RTextBrowser : public QTextBrowser
{

    Q_OBJECT

    protected:

        //! Clear button enabled.
        bool hasClearButton;
        //! Clear button.
        QPushButton *clearButton;

    public:

        //! Constructor.
        explicit RTextBrowser(bool hasClearButton, QWidget *parent = nullptr);

    protected:

        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event) override;
        
};

#endif // RGL_TEXT_BROWSER_H
