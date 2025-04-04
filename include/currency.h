/*
 * This file is part of Coffeekitty.
 * 
 * Copyright (C) 2025 Alexander Hahn
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the European Union Public License (EUPL), version 1.2.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * European Union Public License for more details.
 * 
 * You should have received a copy of the European Union Public License
 * along with this program. If not, see <https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12>.
 */

#ifndef CURRENCY_H
#define CURRENCY_H

#include <stdbool.h>

typedef struct Currency{
    char isoname[4];
    bool prefix;
    int subunit_digits;
    char decimal;
} Currency;

typedef struct CurrencyValue{
    int value; // in cents or whatever is the subunit
    Currency *currency;
} CurrencyValue;

Currency *create_currency(char* isoname, bool prefix, int subunit_digits, char decimal);
Currency *create_currency_from_locale();
CurrencyValue *create_currency_value(int value, Currency *currency);
void free_currency(Currency *c);
void free_currency_value(CurrencyValue *cv);
CurrencyValue* copy_currency_value(CurrencyValue *cv);

const char* format_currency_value_color_prefix(CurrencyValue *cv);
const char* format_currency_value_color_suffix(CurrencyValue *cv);
const char* format_currency_value(CurrencyValue* cv, bool color, bool affix);
CurrencyValue* ftocv(float value, Currency *currency);

void add_to_currency_value(CurrencyValue *cv1, CurrencyValue *cv2);
void sub_from_currency_value(CurrencyValue *cv1, CurrencyValue *cv2);
void mul_currency_value(CurrencyValue *cv, int factor);
void div_currency_value(CurrencyValue *cv, int divisor);
void negative_currency_value(CurrencyValue *cv);
CurrencyValue* new_negative_currency_value(CurrencyValue *cv);

#endif