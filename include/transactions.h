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
    PERSON_PAYS_DEBT = 0,
    PERSON_BUYS_MISC = 1,
    PERSON_DRINKS_COFFEE = 2,
    KITTY_BUY_COFFEE = 3,
    KITTY_CONSUME_PACK = 4
};

typedef struct BalanceDelta {
    CurrencyValue* cv;
    Person* target;
    struct BalanceDelta* next;
} BalanceDelta;

typedef struct PacksDelta {
    int packs;
    struct PacksDelta* next;
} PacksDelta;

typedef struct CounterDelta {
    int counter;
    Person* target;
    struct CounterDelta* next;
} CounterDelta;

typedef struct Transaction {
    enum transaction_type type;
    BalanceDelta* balance_delta_head;
    PacksDelta* packs_delta_head;
    CounterDelta* counter_delta_head;
    struct Transaction* next;
} Transaction;

void person_pays_debt(Kitty* kitty, Person* person,  CurrencyValue* payment);
void person_buys_misc(Kitty* kitty, Person* person, CurrencyValue* cost);
void person_drinks_coffee(Kitty* kitty, Person* person, int amount);
void buy_coffee(Kitty* kitty, int amount, CurrencyValue* cost);
void consume_pack(Kitty* kitty);

void calculate_thirst(Person* person);

#endif