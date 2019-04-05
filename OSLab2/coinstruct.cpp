#include "coinstruct.h"


CoinStruct::CoinStruct()
    : id(), name(), era(), year(), century(), isRare(), number()
{

}

CoinStruct::CoinStruct(const CoinItem &item)
{
    id      = item.id;
    name    = item.name;
    era     = item.era;
    year    = item.year;
    century = item.century;
    isRare  = item.isRare;
    number  = item.number;
}

QListWidgetItem *CoinStruct::toListWidgetItem() const
{
    QString label;

    (label += name) += "\t";
    if (era == Era::AfterChrist)
    {
        label += QString::number(year);
    }
    else
    {
        label += QString::number(century) += " в. до н. э.";
    }

    if (isRare)
    {
        (label += "\t") += QString::number(number) += " шт.";
    }

    QListWidgetItem* newItem = new QListWidgetItem(label);
    newItem->setData(Qt::UserRole, id);
    return newItem;
}
