#ifndef ENUMHADLER_H
#define ENUMHADLER_H

/*!
 * \brief Перечисление стран
 */
enum class Country : unsigned char
{
    RussianEmpire = 0,
    USSR          = 1,
    Russia        = 2
};

/*!
 * \brief Перечисление эр
 */
enum class Era : unsigned char
{
    BeforeChrist = 0,
    AfterChrist  = 1
};

/*!
 * \brief Функция для перевода элемента Country в соответствующий индекс интерфейсного перечисления
 * \param country - элемент Country
 * \return - соответствующий индекс
 */
int countryToIndex(Country country);

/*!
 * \brief Функция для перевода индекса из интерфейсного перечисления в соответствующий элемент Era
 * \param index - индекс из интерфейсного перечисления
 * \return - соответствующий элемент Era
 */
Era indexToEra(int index);

/*!
 * \brief Функция для перевода элемента Era в соответствующий индекс интерфейсного перечисления
 * \param era - элемент Era
 * \return - соответствующий индекс
 */
int eraToIndex(Era era);

/*!
 * \brief Функция для перевода индекса из интерфейсного перечисления в элемент Country
 * \param index - индекс интерфейского перечисления
 * \return - соответствующий элемент Country
 */
Country indexToCountry(int index);

#endif // ENUMHADLER_H
