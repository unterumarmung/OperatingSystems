#ifndef COINSTRUCT_H
#define COINSTRUCT_H

#include<coinitem.h>
#include<QListWidgetItem>

/*!
 * \brief структура записи в браузере
 */
struct CoinStruct
{
    quint32 id;   ///индентификатор записи
    QString name; ///наименование
    Era era;      ///эра
    int year;     ///год
    int century;  ///век
    bool isRare;  ///раритетна ли
    int number;   ///количество монет

    /*!
     * \brief Конструктор по умолчанию
     */
    CoinStruct();
    /*!
     * \brief Конструктор от основного класса записи
     * \param item - запись
     */
    CoinStruct(const CoinItem& item);
    /*!
     * \brief Функция, создающая соответствующий элемент списка
     * \return - элемент списка для данной записи
     */
    QListWidgetItem* toListWidgetItem() const;
};

#endif // COINSTRUCT_H
