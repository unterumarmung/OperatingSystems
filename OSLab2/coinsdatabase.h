#ifndef COINSDATABASE_H
#define COINSDATABASE_H
#include<QMap>
#include<QVector>
#include<QString>
#include<QListWidgetItem>
#include<coinitem.h>
#include <QFile>
#include <coinstruct.h>
#include <QPair>

class CoinsDatabase
{
public:
    CoinsDatabase();
    /*!
     * \brief Получить количество элементов в базе данных
     * \return - количество элементов
     */
    int count() const;
    /*!
     * \brief Добавить запись в базу
     * \param record - запись
     * \return - позиция записи в базе
     */
    QPair<int, quint32> append(CoinItem record);
    /*!
     * \brief Удалить запись с данным id
     * \param id - id записи для удаления
     */
    void remove(quint32 id);
    /*!
     * \brief Обновить запись в форме
     * \param record - запись
     * \return - новая позиция записи в базе
     */
    int update(CoinItem record);
    /*!
     * \brief Обратиться к записи с конкретным id
     * \param id - id записи
     * \return - запись
     */
    CoinItem record(quint32 id) const;
    /*!
     * \brief Вернуть массив структур всех элементов в отсортированном порядке
     * \return - массив структур
     */
    const QVector<CoinStruct> records() const;
    /*!
     * \brief Сохранить текущее состояение базы в файл
     * \param filename - имя файла
     * \return - успешность сохранения
     */
    bool save(QString filename = "");
    /*!
     * \brief Загрузить базу из файла
     * \param filename - имя файла
     * \return - успешность загрузки
     */
    bool load(QString filename = "");
    /*!
     * \brief Очистить всю базу
     */
    void clear();
    /*!
     * \brief Показывать была ли изменена база
     * \return - флаг об изменении
     */
    bool isModified() const;

    /*!
     * \brief Установить имя файла сохранения
     * \param name - имя файла
     */
    void setDatabaseFilename(QString name);
protected:
    /*!
     * \brief Вставить элемент в базу
     * \param item - элемент
     * \return - индекс вставленной записи в порядке сортировки
     */
    int insert(const CoinItem& item);
private:
    ///словарь: id записи -> запись
    QMap<quint32, CoinItem> db;
    ///массив id записей в отсортированном порядке
    QVector<quint32>        ids;
    ///масимальное количество id
    quint32                 maxId    = 0;
    ///флаг о измненённости модели
    bool                     modified = false;

    QString dbFilename = "coins_database.db";
};
#endif // COINSDATABASE_H
