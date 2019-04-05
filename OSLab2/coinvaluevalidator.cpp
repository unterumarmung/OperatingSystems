#include "coinvaluevalidator.h"

CoinValueValidator::CoinValueValidator()
{

}

/*!
 * \brief метод валидации строки по номиналу монеты
 * \param str - строка для валидации
 * \param pos - позиция курсора
 * \return - состояние правильности строки
 */
QValidator::State CoinValueValidator::validate(QString &str, int &pos) const
{
    QValidator::State state = QValidator::Acceptable;

    //первичная проверка строки на Invalid
    for (int i = 0; i < str.length() && state != QValidator::Invalid; ++i)
    {
        if (!(str[i].isLetterOrNumber() || str[i] == ' '))
        {
            state = QValidator::Invalid;
        }
    }

    //вычисление длины численной части
    int index = 0;          //текущий индекс
    int digitPartLength = 0;

    while (str[index].isDigit())
    { //пока текущий символ - цифра
        index += 1;
        digitPartLength += 1;
    }

    //вычисление длины пробельной части
    int spacePartLength = str.count(' ');
    index += spacePartLength;

    //вычисление длины буквенной части
    int letterPartLength = 0;
    while (str[index].isLetter())
    { //пока текущий символ - буква
        index += 1;
        letterPartLength += 1;
    }

    if (digitPartLength == 0 || spacePartLength == 0 || letterPartLength == 0)
    { //одна из частей номинала отсутствует
        state = QValidator::Intermediate;
    }
    if (spacePartLength == 1 && letterPartLength == 0 && index < str.length() - 1)
    { //после пробела пытаются ввести не букву
        state = QValidator::Invalid;
    }
    if (letterPartLength != 0 && index != str.length())
    { //после букв пытаются ввести не буквы
        state = QValidator::Invalid;
    }
    if (spacePartLength > 1)
    { //количество пробелов - больше одного
        state = QValidator::Invalid;
    }
    return state;
}
