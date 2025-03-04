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

#include "currency.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "colors.h"

Currency *create_currency(char* isoname, bool prefix, int subunit_digits, char decimal)
{
    Currency *c = malloc(sizeof(Currency));
    strncpy(c->isoname, isoname, 4);
    c->prefix = prefix;
    c->subunit_digits = subunit_digits;
    c->decimal = decimal;
    return c;
}

CurrencyValue *create_currency_value(int value, Currency *currency)
{
    CurrencyValue *cv = malloc(sizeof(CurrencyValue));
    cv->value = value;
    cv->currency = currency;
    return cv;
}

void free_currency(Currency *c)
{
    free(c);
}

void free_currency_value(CurrencyValue *cv)
{
    // currency is not freed, as it is a shared resource
    free(cv);
}

CurrencyValue* copy_currency_value(CurrencyValue *cv)
{
    return create_currency_value(cv->value, cv->currency);
}

/* printing */

const char* format_currency_value_color_prefix(CurrencyValue *cv)
{
    (void)cv;

    if (cv->value < 0)
        return ANSI_RED;
    else if (cv->value > 0)
        return ANSI_GREEN;
    else
        return ANSI_RESET;
}

const char* format_currency_value_color_suffix(CurrencyValue *cv)
{
    (void)cv;

    return ANSI_RESET;
}

const char* format_currency_value(CurrencyValue *cv, bool add_color, bool add_affix)
{
    _Thread_local static char str[128];

    int subunit_size = 1;
    for (int i = 0; i < cv->currency->subunit_digits; i++)
        subunit_size *= 10;

    char sign_char = cv->value < 0 ? '-' : ' ';
    int subunit_value = abs(cv->value % subunit_size);
    int currency_value = abs(cv->value / subunit_size);

    char color_ctrl[6] = {0};
    char color_reset[6] = {0};
    if (add_color) {
        strcpy(color_reset, ANSI_RESET);
        if (cv->value < 0)
            strcpy(color_ctrl, ANSI_RED);
        else if (cv->value > 0)
            strcpy(color_ctrl, ANSI_GREEN);
        else
            strcpy(color_ctrl, ANSI_RESET);
    }

    char affix[sizeof(cv->currency->isoname) + 1] = {0};
    char *prefix = "";
    char *suffix = "";
    if (cv->currency->prefix) {
        snprintf(affix, sizeof(affix), "%s ", cv->currency->isoname);
        prefix = affix;
    } else {
        snprintf(affix, sizeof(affix), " %s", cv->currency->isoname);
        suffix = affix;
    }

    if (!add_affix) {
        prefix = "";
        suffix = "";
    }

    snprintf(str, sizeof(str), "%s%s%c%i%c%0*d%s%s",
    color_ctrl, prefix,
    sign_char, currency_value, cv->currency->decimal, cv->currency->subunit_digits, subunit_value,
    suffix, color_reset);
    return str;
}

CurrencyValue* ftocv(float value, Currency *currency)
{
    int subunit_size = 1;
    for (int i = 0; i < currency->subunit_digits; i++)
        subunit_size *= 10;
    return create_currency_value(round(subunit_size*value), currency);
}

/* mathematical operations */

void add_to_currency_value(CurrencyValue *cv1, CurrencyValue *cv2)
{
    cv1->value += cv2->value;
}

void sub_from_currency_value(CurrencyValue *cv1, CurrencyValue *cv2)
{
    cv1->value -= cv2->value;
}

void mul_currency_value(CurrencyValue *cv, int factor)
{
    cv->value *= factor;
}

void div_currency_value(CurrencyValue *cv, int divisor)
{
    cv->value /= divisor;
}