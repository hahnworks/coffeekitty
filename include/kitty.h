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

#ifndef KITTY_H
#define KITTY_H

#include "settings.h"
#include "currency.h"
#include "person.h"

typedef struct Kitty{
    CurrencyValue *balance;
    CurrencyValue *price;

    int packs;
    int counter;

    Person *persons;
    Settings *settings;
} Kitty;

Kitty *create_kitty(int balance, int price, int packs, int counter, Settings* settings, Person* persons);
void free_kitty(Kitty *k);

#endif