#ifndef COINVALUEVALIDATOR_H
#define COINVALUEVALIDATOR_H

#include <QValidator>

class CoinValueValidator : public QValidator
{
public:
    CoinValueValidator();

    QValidator::State validate(QString&, int&) const;
};

#endif // COINVALUEVALIDATOR_H
