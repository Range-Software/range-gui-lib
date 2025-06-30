#ifndef RGL_FILE_TAGS_DIALOG_H
#define RGL_FILE_TAGS_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QStringList>
#include <QLineEdit>
#include <QMap>

#include <rcl_file_info.h>

#include "rgl_flow_layout.h"
#include "rgl_label_widget.h"

class RFileTagsDialog : public QDialog
{
    Q_OBJECT

    protected:

        QStringList tags;

        QMap<QString,RLabelWidget*> labelWidgets;

        RFlowLayout *labelLayout;
        QLineEdit *lineEdit;
        QPushButton *addButton;
        QPushButton *okButton;

    public:

        //! Constructor.
        explicit RFileTagsDialog(const RFileInfo &fileInfo, QWidget *parent = nullptr);

        //! Return tags.
        QStringList getTags() const;

    protected:

        void addLabel(const QString &text);

    protected slots:

        void onCloseLabelClicked(const QVariant &data);
        void onTextEdited(const QString &text);
        void onAddClicked();

};

#endif // RGL_FILE_TAGS_DIALOG_H
