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

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include "person.h"
#include "currency.h"
#include "kitty.h"

enum transaction_type {
    PERSON_PAYS_DEBT,
    PERSON_BUYS_MISC,
    PERSON_DRINKS_COFFEE,
    KITTY_BUY_COFFEE,
    KITTY_CONSUME_PACK
};

enum balance_delta_target {
    PERSON,
    KITTY
};

typedef struct BalanceDelta {
    CurrencyValue* delta;
    enum balance_delta_target target;
    struct BalanceDelta* next;
} BalanceDelta;

typedef struct Transaction {
    enum transaction_type type;
    BalanceDelta* delta_head;
    struct Transaction* next;
} Transaction;


void person_pays_debt(Person* person, Kitty* kitty, CurrencyValue* payment);
void person_buys_misc(Person* person, CurrencyValue* cost);
void person_drinks_coffee(Person* person, Kitty* kitty, int amount);
void buy_coffee(Kitty* kitty, int amount, CurrencyValue* cost);
void calculate_thirst(Person* person);
void consume_pack(Kitty* kitty);

#endif