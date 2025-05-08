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

#include "kitty.h"

#include <stdlib.h>

#include "settings.h"
#include "currency.h"
#include "person.h"
#include "transactions.h"

Kitty *create_kitty(int balance, int price, int packs, int counter, Settings *settings, Person *persons, Transaction *transactions)
{
    Kitty *k = malloc(sizeof(Kitty));

    k->balance = currency_value_alloc(balance, settings->currency);
    k->price = currency_value_alloc(price, settings->currency);

    k->counter = counter;
    k->packs = packs;

    k->settings = settings;
    k->persons = persons;
    k->transactions = transactions;
    return k;
}

void kitty_free(Kitty *k)
{
    currency_value_free(k->balance);
    currency_value_free(k->price);
    free(k);
}