#ifndef RGL_PROGRESS_BAR_H
#define RGL_PROGRESS_BAR_H

#include <QProgressBar>
#include <QTimer>

class RProgressBar : public QProgressBar
{

    Q_OBJECT

    protected:

        //! Pulse timer.
        QTimer *pulseTimer;
        //! Custom message.
        QString message;
        //! Autohide flag.
        bool autoHide;

    public:

        //! Constructor.
        explicit RProgressBar(QWidget *parent = nullptr);

        //! Start pulsing.
        void startPulse(void);

        //! Stop pulsing.
        void stopPulse(void);

        //! Set progress bar value.
        void setValue(int value);

        //! Set message.
        void setMessage(const QString &message);

        //! Set autohide.
        void setAutoHide(bool autoHide);

    public slots:

        //! Hide widget.
        void hide(void);

    private slots:

        //! Increase pulse value.
        void pulse(void);

};

#endif // RGL_PROGRESS_BAR_H
