#ifndef RGL_TERRITORY_COMBO_BOX_H
#define RGL_TERRITORY_COMBO_BOX_H

#include <QComboBox>

class RTerritoryComboBox : public QComboBox
{

    Q_OBJECT

    public:

        //! Constructor
        explicit RTerritoryComboBox(const QString &currentTerritoryCode = QString(), QWidget *parent = nullptr);

        //! Return current territory code.
        QString currentTerritoryCode() const;

        //! Set current territory code.
        void setCurrentTerritoryCode(const QString &territoryCode);

    private:

        //! Find all territory codes with territory names.
        static QMap<QString,QString> findTerritoryCodes();

    private slots:

        //! Current combo box changed.
        void onCurrentIndexChanged(int index);

    signals:

        void territorrySelected(QString territory);

};

#endif // RGL_TERRITORY_COMBO_BOX_H
