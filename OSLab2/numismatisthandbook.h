#ifndef NUMISMATISTHANDBOOK_H
#define NUMISMATISTHANDBOOK_H

#include <QMainWindow>
#include <coinitem.h>
#include "enumhandler.h"
#include <QDate>
#include <coinvaluevalidator.h>
#include <QMessageBox>
#include <QTextCodec>
#include <QListWidget>
#include <coinsdatabase.h>
#include <QFileDialog>
#include <QCloseEvent>
#include <QIcon>
#include <QKeySequence>
#include <QTimer>

namespace Ui {
class NumismatistHandbook;
}

class NumismatistHandbook : public QMainWindow
{
    Q_OBJECT

public:
    explicit NumismatistHandbook(QWidget *parent = nullptr);

    /*!
     * \brief Деструктор главного окна
     */
    ~NumismatistHandbook();

protected:
    //определение синонимов для статуса сохранения измененного файла
    using SafeState = QMessageBox::StandardButton;
    const SafeState SafeAccepted  = QMessageBox::StandardButton::Yes;
    const SafeState SafeDeclined  = QMessageBox::StandardButton::No;
    const SafeState EventCanceled = QMessageBox::StandardButton::Cancel;

    ///максимальное количество элементов
    static constexpr int    MAX_ITEMS_COUNT = 100;
    ///индекс текущего элемента
    int                     currentItemIndex = -1;
    ///интерфейс
    Ui::NumismatistHandbook *ui;
    ///массив всех элементов формы
    QVector<QWidget*>       allFormWidgets;
    ///валидатор для номинала монеты
    QValidator*             coinValueValidator;
    ///имя текущего файла
    QString                 currentFilename;
    ///элементы текущего файла
    CoinsDatabase           model;
    ///
    QLabel* label;
    /*!
     * \brief Взять текущие значения полей из формы и создать на их основе элемент
     * \return - созданный элемент
     */
    CoinItem fromFormToItem();
    /*!
     * \brief Загрузить элемент в форму
     * \param item - данный элемент
     */
    void fromItemToForm(const CoinItem& item = CoinItem());
    /*!
     * \brief Удалить элемент их массива и формы
     * \param index - индекс удаляемого элемента
     */
    void deleteItemEverywhere(int index);
    /*!
     * \brief Вставить элемент в массив и форму
     * \param item - вставляемый элемент
     * \return - позиция нового элемента
     */
    int insertItemEverywhere(const CoinItem& item);

    /*!
     * \brief Метод, обновляющий форму
     */
    void updateForm();

    /*!
     * \brief Функция инициализации главного окна (не используется)
     * \param enable - включить или выключить окно
     */
    void startInit(bool enable);

    /*!
     * \brief Показывает изменился ли файл или нет: новый и не пустой или неновый и изменённый
     * \return - изменился или нет
     */
    bool fileChanged();

    /*!
     * \brief Всплывающее окошко о сохранении файла
     * \param includeCancelButton - добавлять ли в окно кнопку "отмена"
     * \return - состояние, которое выбрал пользователь - сохранить, не сохранить или отменить действие
     */
    SafeState askToSafe(bool includeCancelButton);

    /*!
     * \brief Перегруженная виртуальная функция, вызывающаяся при закрытии окна
     * \param event - событие закрытия главного окна
     */
    void closeEvent(QCloseEvent* event);

    /*!
     * \brief Выводит текущее имя файла
     */
    void showCurrentFilename();

    std::atomic<int> saveCount;

    void parseConfigFile();
    QMap<QString, QString> config;

private slots:
    /*!
     * \brief Слот, ответственный за включение/выключение поля количества монет
     */
    void rarityHandlerSlot();
    /*!
     * \brief Метод/слот загружает данные текущего элемента в форму
     */
    void loadDataToForm();
    /*!
     * \brief Слот ответственный за работу составного поля "Год выпуска"
     * \param index - индекс текущего элемента интерфейсного перечисления
     */
    void yearHandlerSlot(int index = 0);
    /*!
     * \brief Слот для загрузки данных из формы в объект
     */
    void saveItemSlot();
    /*!
     * \brief Заполнение базы данных сэмплами
     */
    void fillSlot();
    /*!
     * \brief Обработка нажатия на кнопку "Создать"
     */
    void createItemSlot();
    /*!
     * \brief Обработка нажатия на кнопку "Удалить"
     */
    void deleteItemSlot();
    /*!
     * \brief Обработка изменения текущего элемента в QListWidget
     * \param currentRow - новая позиция
     */
    void rowChangedSlot(int currentRow = -1);

    /*!
     * \brief Обработка сохранения файла
     */
    void saveFileSlot();

    /*!
     * \brief Обработка сохранения в новый файл
     */
    void saveAsFileSlot();

    /*!
     * \brief Обработка открытия файла
     */
    void openFileSlot();

    /*!
     * \brief Обработка создания нового файла
     */
    void createFileSlot();
};

/*!
 * \brief Для массива виджетов установить их активность
 * \param widgets - массив указателей на виджеты
 * \param value - включен или нет
 */
void setWidgetsEnabled(const QVector<QWidget*>& widgets, bool value);

#endif // NUMISMATISTHANDBOOK_H
