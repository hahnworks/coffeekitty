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

Currency *currency_alloc(char* isoname, bool prefix, int subunit_digits, char decimal);
Currency *create_currency_from_locale();
CurrencyValue *currency_value_alloc(int value, Currency *currency);
void currency_free(Currency *c);
void currency_value_free(CurrencyValue *cv);
CurrencyValue* currency_value_copy(CurrencyValue *cv);

const char* currency_value_format_color_prefix(CurrencyValue *cv);
const char* currency_value_format_color_suffix(CurrencyValue *cv);
const char* currency_value_format(CurrencyValue* cv, bool color, bool affix);
CurrencyValue* ftocv(float value, Currency *currency);

void currency_value_add(CurrencyValue *cv1, CurrencyValue *cv2);
void currency_value_sub(CurrencyValue *cv1, CurrencyValue *cv2);
void currency_value_mul(CurrencyValue *cv, int factor);
void currency_value_div(CurrencyValue *cv, int divisor);
void currency_value_negative(CurrencyValue *cv);
CurrencyValue* currency_value_new_negative(CurrencyValue *cv);

#endif