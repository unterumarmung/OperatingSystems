#include "coinsdatabase.h"

CoinsDatabase::CoinsDatabase()
{

}

int CoinsDatabase::count() const
{
    return db.count();
}

QPair<int, quint32> CoinsDatabase::append(CoinItem record)
{
    modified = true;

    quint32 id = maxId++; //присваиваем записи незанятый айди и увеличиваем число айди
    record.id = id;
    int index = insert(record);

    return qMakePair(index, id);
}

int CoinsDatabase::insert(const CoinItem &record)
{
    db[record.id] = record;

    QMutableVectorIterator<quint32> i(ids);

    int index = 0; //Индекс вставки записи в браузер с учётом сортировки

    while (i.hasNext() && db[i.next()] < record)
        index++;

    if (ids.isEmpty())
    { // база пустая
        ids.append(record.id);
    }
    else if (db[i.value()] < record)
    {
        i.insert(record.id);
    }
    else
    {
        i.previous();
        i.insert(record.id);
    }

    return index;
}

void CoinsDatabase::remove(quint32 id)
{
    modified = true;
    db.remove(id); //удалить из БД
    ids.removeOne(id); //удалить из списка айдишников
}

int CoinsDatabase::update(CoinItem record)
{
    modified = true;

    //временно сохранить запись и удалить из базы данных

    CoinItem tmp = record;        //временная запись для хранения данных
    quint32 oldId = tmp.id;      //сохраняем id, чтобы оставить его прежним у новой записи
    remove(oldId);                //удаляем запись по id

    return insert(tmp);
}

CoinItem CoinsDatabase::record(quint32 id) const
{
    return db[id];
}

const QVector<CoinStruct> CoinsDatabase::records() const
{
    QVector<CoinStruct> vector;
    QVectorIterator<quint32> i(ids);

    while (i.hasNext())
    {
        vector.append(db[i.next()]);
    }

    return vector;
}

bool CoinsDatabase::save(QString filename)
{
    bool opened = false; //успешность открытия файла

    /*
    if (!filename.isEmpty())
    {
        QFile file(filename);
        opened = file.open(QIODevice::WriteOnly);
        modified = false;
        QDataStream output(&file);

        output << db << ids << maxId;

        file.close();
    }
    */

    if (!dbFilename.isEmpty())
    {
        LPTSTR str = (LPTSTR) dbFilename.utf16();
        HANDLE file = CreateFile(str, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        opened = file != INVALID_HANDLE_VALUE;

        if (opened)
        {
            DWORD bytesBeenWritten;
            int count = this->count();
            WriteFile(file, &count, sizeof(int), &bytesBeenWritten, nullptr);
            WriteFile(file, &maxId, sizeof(quint32), &bytesBeenWritten, nullptr);

            for (auto i = ids.begin(); i != ids.end(); ++i)
            {
                CoinItem current = db[*i];
                current.saveToFile(file);
            }

            modified = true;
        }

        CloseHandle(file);
    }
    return opened;
}

bool CoinsDatabase::load(QString filename)
{
    bool opened = false; //успешность открытия файла

    /*
    if (!filename.isEmpty()) //если имя файла не пустое
    {
        //создаем файл
        QFile file(filename);

        //открываем файл только для чтения
        opened = file.open(QIODevice::ReadOnly);

        if (opened)
        { //если открыт успешно
            modified = false;

            QDataStream output(&file);

            output >> db >> ids >> maxId;

            file.close();
        }
    }
    */

    if (!dbFilename.isEmpty())
    {
        LPTSTR str = (LPTSTR) dbFilename.utf16();
        HANDLE file = CreateFile(str, GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        opened = file != INVALID_HANDLE_VALUE;

        if (opened)
        {
            DWORD bytesRead;
            int count = 0;
            ReadFile(file, &count, sizeof(int), &bytesRead, nullptr);
            ReadFile(file, &maxId, sizeof(quint32), &bytesRead, nullptr);

            for (int i = 0; i < count; ++i)
            {
                CoinItem temp;
                temp.readFromFile(file);
                insert(temp);
            }

            modified = true;
        }

        CloseHandle(file);
    }
    return opened;
}

void CoinsDatabase::clear()
{
    modified = true;
    maxId    = 0;
    db.clear();
    ids.clear();
}

bool CoinsDatabase::isModified() const
{
    return modified;
}

void CoinsDatabase::setDatabaseFilename(QString name)
{
    dbFilename = name;
}



