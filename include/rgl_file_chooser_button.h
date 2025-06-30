#ifndef RGL_FILE_CHOOSER_BUTTON_H
#define RGL_FILE_CHOOSER_BUTTON_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class RFileChooserButton : public QWidget
{

    Q_OBJECT

    public:

        enum Type
        {
            SaveFile = 0,
            OpenFile,
            Directory
        };

    protected:

        Type type;
        QString caption;
        QString path;
        QString filter;
        QPushButton *pathButton;
        QPushButton *clearButton;
        QString searchDirectory;

    public:

        //! Constructor.
        explicit RFileChooserButton(const QString &labelText,
                                    Type type,
                                    const QString &caption = QString(),
                                    const QString &path = QString(),
                                    const QString &filter = QString(),
                                    QWidget *parent = nullptr);

        //! Return const reference to search directory.
        const QString &getSearchDirectory() const;

        //! Set new search directory.
        void setSearchDirectory(const QString &searchDirectory);

        //! Hide clear button.
        void hideClearButton();

        //! Return selected file name.
        QString getFileName() const;

    public slots:

        //! Set file name.
        void setFileName(const QString &fileName);

        //! Set filter.
        void setFilter(const QString &filter);

        //! On push button clicked.
        void onPathButtonClicked();

        //! On clear button clicked.
        void onClearButtonClicked();

    signals:

        //! File changed signal.
        void fileNameChanged(const QString &fileName);

};

#endif // RGL_FILE_CHOOSER_BUTTON_H
