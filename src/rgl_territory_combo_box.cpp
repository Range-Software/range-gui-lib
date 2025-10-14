#include "rgl_territory_combo_box.h"

RTerritoryComboBox::RTerritoryComboBox(const QString &currentTerritoryCode, QWidget *parent)
    : QComboBox{parent}
{
    QMap<QString, QString> territoryCodes = RTerritoryComboBox::findTerritoryCodes();

    this->setDuplicatesEnabled(false);

    for (QMap<QString, QString>::const_iterator iter=territoryCodes.cbegin();iter!=territoryCodes.cend();++iter)
    {
        this->addItem(iter.value() + " (" + iter.key() + ")",iter.key());
        if (iter.key() == currentTerritoryCode)
        {
            this->setCurrentIndex(this->count() - 1);
        }
    }
    this->model()->sort(0);

    QObject::connect(this,&QComboBox::currentIndexChanged,this,&RTerritoryComboBox::onCurrentIndexChanged);
}

QString RTerritoryComboBox::currentTerritoryCode() const
{
    return this->currentData().toString();
}

void RTerritoryComboBox::setCurrentTerritoryCode(const QString &territoryCode)
{
    this->setCurrentIndex(this->findData(territoryCode));
}

QMap<QString, QString> RTerritoryComboBox::findTerritoryCodes()
{
    QMap<QString, QString> countryCodesMap;

    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyTerritory);
    for(const QLocale &locale : std::as_const(allLocales))
    {
        QString territoryCode = QLocale::territoryToCode(locale.territory()).toUpper();

        if (territoryCode.length() == 2 && !countryCodesMap.contains(territoryCode))
        {
            countryCodesMap.insert(territoryCode,QLocale::territoryToString(locale.territory()));
        }
    }

    return countryCodesMap;
}

void RTerritoryComboBox::onCurrentIndexChanged(int index)
{
    emit this->territorrySelected(this->currentData().toString());
}
