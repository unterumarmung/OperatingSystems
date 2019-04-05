#include "enumhandler.h"


Country indexToCountry(int index)
{
    Country result;

    if (index == 0)
    { // Российскай Империя
        result = Country::RussianEmpire;
    }
    else if (index == 1)
    { // СССР
        result = Country::USSR;
    }
    else
    { // Россия
        result = Country::Russia;
    }

    return result;
}


int countryToIndex(Country country)
{
    int result;

    if (country == Country::Russia)
    { // Страна - Россия
        result = 2;
    }
    else if (country == Country::USSR)
    { // Страна - СССР
        result = 1;
    }
    else
    { // Страна - Россйиская Империя
        result = 0;
    }

    return result;
}


Era indexToEra(int index)
{
    Era result;

    if (index == 0)
    { //до нашкй эры
        result = Era::BeforeChrist;
    }
    else
    {
        result = Era::AfterChrist;
    }

    return result;
}


int eraToIndex(Era era)
{
    int result;

    if (era == Era::BeforeChrist)
    { //до нашей эры
        result = 0;
    }
    else
    {
        result = 1;
    }

    return result;
}
