#include "numismatisthandbook.h"
#include "ui_numismatisthandbook.h"
#include <thread>
#include <chrono>
#include <QXmlStreamReader>

NumismatistHandbook::NumismatistHandbook(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NumismatistHandbook)
{
    ui->setupUi(this);

    //установка соединения между сигналами и элементами, видимость которых не постоянна
    ui->rarityCheckBox->setTristate(false);
    connect(ui->rarityCheckBox, SIGNAL(stateChanged(int)), this, SLOT(rarityHandlerSlot()));
    connect(ui->eraComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(yearHandlerSlot(int)));

    //ставим минимум и максимум для года
    ui->yearSpinBox->setMinimum(1);
    ui->yearSpinBox->setMaximum(QDate::currentDate().year());

    //установка статус бара
    label = new QLabel();
    ui->statusBar->addWidget(label);

    //соединение сигналов с их обработчиками
    connect(ui->savePushButton,   &QAbstractButton::clicked,       this, &NumismatistHandbook::saveItemSlot);
    connect(ui->listWidget,       &QListWidget::itemClicked,       this, &NumismatistHandbook::loadDataToForm);
    connect(ui->fillPushButton,   &QAbstractButton::clicked,       this, &NumismatistHandbook::fillSlot);
    connect(ui->createPushButton, &QAbstractButton::clicked,       this, &NumismatistHandbook::createItemSlot);
    connect(ui->deletePushButton, &QAbstractButton::clicked,       this, &NumismatistHandbook::deleteItemSlot);
    connect(ui->listWidget,       &QListWidget::currentRowChanged, this, &NumismatistHandbook::rowChangedSlot);

    connect(ui->createItemAction, &QAction::triggered, this, &NumismatistHandbook::createItemSlot);
    connect(ui->deleteItemAction, &QAction::triggered, this, &NumismatistHandbook::deleteItemSlot);
    connect(ui->saveItemAction,   &QAction::triggered, this, &NumismatistHandbook::saveItemSlot);
    connect(ui->fillAction,       &QAction::triggered, this, &NumismatistHandbook::fillSlot);
    connect(ui->createAction,     &QAction::triggered, this, &NumismatistHandbook::createFileSlot);
    connect(ui->openAction,       &QAction::triggered, this, &NumismatistHandbook::openFileSlot);
    connect(ui->saveAction,       &QAction::triggered, this, &NumismatistHandbook::saveFileSlot);
    connect(ui->saveAsAction,     &QAction::triggered, this, &NumismatistHandbook::saveAsFileSlot);

    //установка иконок и шорткатов для действий с файлами и главного окна
    this->setWindowIcon(QIcon("D:/Qt_Projects/Labs/Lab4Dudkin/icons/coins.png"));
    ui->createAction->setIcon(QIcon("D:/Qt_Projects/Labs/Lab4Dudkin/icons/create.png"));
    ui->createAction->setShortcut(QKeySequence("Ctrl+N"));
    ui->createAction->setIconText("Ctrl+N");
    ui->openAction->setIcon(QIcon("D:/Qt_Projects/Labs/Lab4Dudkin/icons/open.png"));
    ui->openAction->setShortcut(QKeySequence("Ctrl+O"));
    ui->openAction->setIconText("Ctrl+O");
    ui->saveAction->setIcon(QIcon("D:/Qt_Projects/Labs/Lab4Dudkin/icons/save.png"));
    ui->saveAction->setShortcut(QKeySequence("Ctrl+S"));
    ui->saveAction->setIconText("Ctrl+S");
    ui->saveAsAction->setIcon(QIcon("D:/Qt_Projects/Labs/Lab4Dudkin/icons/save-as.png"));
    ui->saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    ui->saveAsAction->setIconText("Ctrl+Shift+S");

    //валидатор для номинала
    coinValueValidator = new CoinValueValidator();
    ui->valueLineEdit->setValidator(coinValueValidator);

    //добавление всех элементов, которые должны включаться и выключаться
    allFormWidgets << ui->nameLabel        << ui->nameLineEdit
                   << ui->valueLabel       << ui->valueLineEdit
                   << ui->dateGroupBox     << ui->eraLabel
                   << ui->eraComboBox      << ui->yearLabel
                   << ui->yearSpinBox      << ui->eraLabel
                   << ui->eraComboBox      << ui->rarityCheckBox
                   << ui->numberLabel      << ui->numberSpinBox
                   << ui->countryLabel     << ui->countryComboBox
                   << ui->qualitiesLabel   << ui->qualitiesLineEdit
                   << ui->deletePushButton << ui->savePushButton;
    rowChangedSlot(-1);

    ui->fileMenu->setEnabled(false);

    try
    {
        parseConfigFile();
    }
    catch (std::exception& exception)
    {
        QMessageBox::critical(this, "Config file error", exception.what());
        exit(1);
    }
    catch (...)
    {
        QMessageBox::critical(this, "Config file error", "Unknown error occurred");
        exit(1);
    }
    if (!config["DatabaseFilename"].isEmpty())
    {
        model.setDatabaseFilename(config["DatabaseFilename"]);
    }

    std::chrono::seconds savePeriod;
    if (!config["SaveTimeout"].isEmpty())
    {
        savePeriod = std::chrono::seconds(config["SaveTimeout"].toUInt());
    }
    else
    {
        savePeriod = std::chrono::seconds(10);
    }
    model.load();
    updateForm();

    saveCount = 0;

    auto saveoOnTimeoutFunctor = [this, savePeriod]()
    {
        while (true)
        {
            model.save();
            saveCount += 1;
            label->setText("Файл сохранён " + QString::number(saveCount) + " раз");
            std::this_thread::sleep_for(savePeriod);
        }
    };
    std::thread timerThread(saveoOnTimeoutFunctor);
    timerThread.detach();
}


NumismatistHandbook::~NumismatistHandbook()
{
    delete coinValueValidator;
    delete ui;
}

void NumismatistHandbook::closeEvent(QCloseEvent *event)
{
    model.save();
    event->accept();
}

void NumismatistHandbook::showCurrentFilename()
{
    QString message = "Текущий файл: "; ///сообщение о имени файла

    if (currentFilename.isEmpty())
        message += "<Нет имени>";
    else
        message += currentFilename;

    label->setText(message);
}

void NumismatistHandbook::parseConfigFile()
{
    QFile configFile("config.xml");

    bool isOpened = configFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!isOpened)
        throw std::runtime_error("Cannot open config file!");

    QXmlStreamReader xml(&configFile);
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() != "properties")
                config[xml.name().toString()] = xml.readElementText();
        }
    }

    configFile.close();
    if (xml.hasError())
        throw std::runtime_error("Error occurred while parsing config file");
}


/////////////////////////////////////////////////////////////
///////////////          СЛОТЫ             //////////////////
/////////////////////////////////////////////////////////////

void NumismatistHandbook::loadDataToForm()
{
    quint32 currentId = ui->listWidget->currentItem()->data(Qt::UserRole).toUInt();
    fromItemToForm(model.record(currentId));
}

void NumismatistHandbook::rarityHandlerSlot()
{
    bool shouldBeEnabled = ui->rarityCheckBox->isChecked();

    ui->numberSpinBox->setEnabled(shouldBeEnabled);
    ui->numberLabel->setEnabled(shouldBeEnabled);
}

void NumismatistHandbook::yearHandlerSlot(int index)
{
    Era currentEra     = indexToEra(index);
    bool isBeforeCrist = currentEra == Era::BeforeChrist;
    bool isAfterCrist  = currentEra == Era::AfterChrist;

    ui->yearLabel->setEnabled(isAfterCrist);
    ui->yearSpinBox->setEnabled(isAfterCrist);

    ui->centuryLabel->setEnabled(isBeforeCrist);
    ui->centurySpinBox->setEnabled(isBeforeCrist);
}

void NumismatistHandbook::saveItemSlot()
{
    if (ui->valueLineEdit->hasAcceptableInput())
    { //номинал монеты соответствует шаблону
        //загружаем новые данные из формы
        quint32 currentId = ui->listWidget->currentItem()->data(Qt::UserRole).toUInt();
        int oldIndex = ui->listWidget->currentRow();
        CoinItem newItem = fromFormToItem();
        newItem.id = currentId;

        //включаем все виджеты формы
        setWidgetsEnabled(allFormWidgets, true);
        ui->saveItemAction->setEnabled(true);
        ui->deleteItemAction->setEnabled(true);

        int newIndex = model.update(newItem);

        //обновляем форму
        delete ui->listWidget->takeItem(oldIndex);
        ui->listWidget->insertItem(newIndex, CoinStruct(newItem).toListWidgetItem());

        //обновляем текущий индекс и выделяем новую строку текущего элемента
        currentItemIndex = newIndex;
        ui->listWidget->setCurrentRow(currentItemIndex);

        yearHandlerSlot(ui->eraComboBox->currentIndex());
        rarityHandlerSlot();
    }
    else
    {
        //сообщение о том, что номинал монеты не соответсвует нужному формату
        QMessageBox::warning(this, "Корректность номинала монеты", "Номинал монеты введён неверно");
    }
}

void  NumismatistHandbook::fillSlot()
{
    //создание элементов
    QList<CoinItem> sampleList;
    CoinItem sample1(
                /* name      = */ "2 рубля 1725 года",
                /* value     = */ "2 рубля",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1725,
                /* century   = */ 0,
                /* isRare    = */ false,
                /* number    = */ 0,
                /* country   = */ Country::RussianEmpire,
                /* qualities = */ "Без особых качеств"
                );

    CoinItem sample2(
                /* name      = */ "Золотая монета царя Гороха",
                /* value     = */ "1 золотец",
                /* era       = */ Era::BeforeChrist,
                /* year      = */ 0,
                /* century   = */ 5,
                /* isRare    = */ true,
                /* number    = */ 3,
                /* country   = */ Country::RussianEmpire,
                /* qualities = */ "Есть сколы"
                );

    CoinItem sample3(
                /* name      = */ "Пробная монета",
                /* value     = */ "1000 рублей",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1995,
                /* century   = */ 0,
                /* isRare    = */ true,
                /* number    = */ 10,
                /* country   = */ Country::Russia,
                /* qualities = */ "Пробная монета Ленинградского монетного двора"
                );


    CoinItem sample4(
                /* name      = */ "Хрущёвский новодел",
                /* value     = */ "20 копеек",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1952,
                /* century   = */ 0,
                /* isRare    = */ false,
                /* number    = */ 0,
                /* country   = */ Country::USSR,
                /* qualities = */ "специальный чекан"
                );

    CoinItem sample5(
                /* name      = */ "Монетовидный жетон в память кончины Императора Александра III",
                /* value     = */ "1 жетон",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1894,
                /* century   = */ 0,
                /* isRare    = */ false,
                /* number    = */ 0,
                /* country   = */ Country::RussianEmpire,
                /* qualities = */ "Серебро"
                );

    CoinItem sample6(
                /* name      = */ "Золотая царская монета Павла I",
                /* value     = */ "5 рублей",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1801,
                /* century   = */ 0,
                /* isRare    = */ false,
                /* number    = */ 0,
                /* country   = */ Country::RussianEmpire,
                /* qualities = */ "Золото 5,99г"
                );

    CoinItem sample7(
                /* name      = */ "Серебрянная царская монета Петра I",
                /* value     = */ "1 рубль",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1725,
                /* century   = */ 0,
                /* isRare    = */ true,
                /* number    = */ 5,
                /* country   = */ Country::RussianEmpire,
                /* qualities = */ "Без букв"
                );

    CoinItem sample8(
                /* name      = */ "Пробная монета",
                /* value     = */ "3 копейки",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1991,
                /* century   = */ 0,
                /* isRare    = */ true,
                /* number    = */ 10,
                /* country   = */ Country::USSR,
                /* qualities = */ "Стоимость 600 000 рублей"
                );

    CoinItem sample9(
                /* name      = */ "Серебрянная царская монета Иоанна Антоновича",
                /* value     = */ "1 полтина",
                /* era       = */ Era::AfterChrist,
                /* year      = */ 1741,
                /* century   = */ 0,
                /* isRare    = */ true,
                /* number    = */ 2,
                /* country   = */ Country::RussianEmpire,
                /* qualities = */ "Сколоты края, стоимость 8 млн"
                );

    sampleList << sample1 << sample2 << sample3 << sample4 << sample5
               << sample6 << sample7 << sample8 << sample9;

    QMutableListIterator<CoinItem> i(sampleList);
    //вставка элементов в форму
    while (i.hasNext())
    {
        model.append(i.next());
    }

    updateForm();
    ui->listWidget->setCurrentRow(model.count() - 1);
    ui->fillPushButton->setEnabled(false);
}

void NumismatistHandbook::createItemSlot()
{
    if (model.count() < MAX_ITEMS_COUNT)
    {
        //создание нового элемента по умолчанию
        CoinItem newItem;

        //включаем все виджеты
        setWidgetsEnabled(allFormWidgets, true);
        ui->saveItemAction->setEnabled(true);
        ui->deleteItemAction->setEnabled(true);

        //вставляем новый элемент в массив
        auto insertResult = model.append(newItem);
        int newIndex = insertResult.first;
        newItem.id = insertResult.second;

        //обновляем форму
        ui->listWidget->insertItem(newIndex, CoinStruct(newItem).toListWidgetItem());

        //обновляем текущий индекс и выделяем новую строку текущего элемента
        currentItemIndex = newIndex;
        ui->listWidget->setCurrentRow(currentItemIndex);
    }
    else
    {
        //сообщение о том, что невозможно добавить новую запись
        QMessageBox::critical(this, "Создание новой записи", "Превышение допустимого количества элементов!\n"
                                                             "Чтобы добавить новую запись удалите любую существующую.");
    }
}

void NumismatistHandbook::deleteItemSlot()
{
    int currentIndex = ui->listWidget->currentRow();

    if (currentIndex != -1)
    { //выбран существующий элемент
        quint32 currentId = ui->listWidget->currentItem()->data(Qt::UserRole).toUInt();

        //удаляем из модели и формы
        model.remove(currentId);
        delete ui->listWidget->takeItem(currentIndex);
    }

    //выбор нового индекса
    int newIndex = currentIndex;
    if (currentIndex >= model.count())
        newIndex -= 1;

    ui->listWidget->setCurrentRow(newIndex);
}

void NumismatistHandbook::rowChangedSlot(int currentRow)
{
    bool enabler;

    if (currentRow != -1)
    { //выбран существующий элемент
        enabler = true;
        quint32 currentId = ui->listWidget->currentItem()->data(Qt::UserRole).toUInt();

        currentItemIndex = currentRow;
        fromItemToForm(model.record(currentId));
    }
    else
    { //строка сбросилась
        enabler = false;
        fromItemToForm(CoinItem());
    }

    setWidgetsEnabled(allFormWidgets, enabler);
    ui->saveItemAction->setEnabled(enabler);
    ui->deleteItemAction->setEnabled(enabler);
    ui->fillPushButton->setEnabled(! enabler);
    ui->fillAction->setEnabled(! enabler);

    if (currentRow != -1)
    {
        yearHandlerSlot(ui->eraComboBox->currentIndex());
        rarityHandlerSlot();
    }
}

void NumismatistHandbook::saveFileSlot()
{
    if (!currentFilename.isEmpty())
        model.save(currentFilename);
    else
        saveAsFileSlot();
}

void NumismatistHandbook::saveAsFileSlot()
{
    QString newFilename = QFileDialog::getSaveFileName(this, "Сохранить как", "/", "Файлы монет (*.coins)");

    if (!newFilename.isEmpty())
    {
        currentFilename = newFilename;
        model.save(currentFilename);
    }
    showCurrentFilename();
}

void NumismatistHandbook::openFileSlot()
{
    //сохранение текущего файла, если нужно
    if (fileChanged())
    {
        if (askToSafe(/*includeCancelButton = */false) == SafeAccepted)
            saveFileSlot();
    }

    //открываем новый файл
    QString newFilename = QFileDialog::getOpenFileName(this, "Открыть файл", "/", "Файлы монет (*.coins)");
    if (!newFilename.isEmpty())
    {
        currentFilename = newFilename;

        model.load(currentFilename);
        updateForm();
        showCurrentFilename();
    }


}

void NumismatistHandbook::createFileSlot()
{
    if (fileChanged())
    {
        if (askToSafe(/*includeCancelButton = */false) == SafeAccepted)
            saveFileSlot();
    }

    currentFilename = "";
    model.clear();
    ui->listWidget->clear();

    showCurrentFilename();
}

/////////////////////////////////////////////////////////////
///////////    Методы загрузки из/в форму     //////////////
////////////////////////////////////////////////////////////
CoinItem NumismatistHandbook::fromFormToItem()
{
    return CoinItem (
                /* name      = */ ui->nameLineEdit->text(),
                /* value     = */ ui->valueLineEdit->text(),
                /* era       = */ indexToEra(ui->eraComboBox->currentIndex()),
                /* year      = */ ui->yearSpinBox->value(),
                /* century   = */ ui->centurySpinBox->value(),
                /* isRare    = */ ui->rarityCheckBox->isChecked(),
                /* number    = */ ui->numberSpinBox->value(),
                /* country   = */ indexToCountry(ui->countryComboBox->currentIndex()),
                /* qualities = */ ui->qualitiesLineEdit->text()
                );
}

void NumismatistHandbook::fromItemToForm(const CoinItem &item)
{
    ui->nameLineEdit     ->setText(item.name);
    ui->valueLineEdit    ->setText(item.value);
    ui->eraComboBox      ->setCurrentIndex(eraToIndex(item.era));
    ui->yearSpinBox      ->setValue(item.year);
    ui->centurySpinBox   ->setValue(item.century);
    ui->rarityCheckBox   ->setChecked(item.isRare);
    ui->numberSpinBox    ->setValue(item.number);
    ui->countryComboBox  ->setCurrentIndex(countryToIndex(item.country));
    ui->qualitiesLineEdit->setText(item.qualities);
    rarityHandlerSlot();
}

/////////////////////////////////////////////////////////////
///////////       Работа с QListWidget        //////////////
////////////////////////////////////////////////////////////

void NumismatistHandbook::updateForm()
{
    //удаление всех элементов из формы
    for (int i = ui->listWidget->count() - 1; i != -1; i = ui->listWidget->count() - 1)
    {
        //удаление элемента из формы (takeItem) и из памяти (delete)
        delete ui->listWidget->takeItem(i);
    }

    //загрузка всех элементов в форму
    QVectorIterator<CoinStruct> i = model.records();
    while (i.hasNext())
    {
        ui->listWidget->addItem(i.next().toListWidgetItem());
    }
}

void NumismatistHandbook::startInit(bool enable)
{
    ui->listWidget->setEnabled(enable);
    ui->createPushButton->setEnabled(enable);
    ui->fillPushButton->setEnabled(enable);
    ui->createItemAction->setEnabled(enable);
    ui->fillAction->setEnabled(enable);
}

bool NumismatistHandbook::fileChanged()
{
    //файл был изменен и не является созданным и не сохраннёным файлом без записей
    return model.isModified() && !(currentFilename.isEmpty() && model.count() == 0);
}

NumismatistHandbook::SafeState NumismatistHandbook::askToSafe(bool includeCancelButton)
{

    QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No;
    if (includeCancelButton)
        buttons |= QMessageBox::Cancel;

    QMessageBox::StandardButton selectedButton =
            QMessageBox::question(this, "База данных", "Сохранить изменения в текущем файле?", buttons);

    return selectedButton;
}


/////////////////////////////////////////////////////////////
///////////             Остальное             //////////////
////////////////////////////////////////////////////////////
void setWidgetsEnabled(const QVector<QWidget*>& widgets, bool value)
{
    QVectorIterator<QWidget*> i(widgets);
    while (i.hasNext())
    {
        i.next()->setEnabled(value);
    }
    for (auto i = widgets.begin(); i != widgets.end(); ++i)
        (*i)->setEnabled(value);
}
