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

enum transaction_type {
    PERSON_PAYS_DEBT = 0,
    PERSON_BUYS_MISC = 1,
    PERSON_DRINKS_COFFEE = 2,
    KITTY_BUY_COFFEE = 3,
    KITTY_CONSUME_PACK = 4,
    UNDO = 5,
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
    long timestamp;
    BalanceDelta* balance_delta_head;
    PacksDelta* packs_delta_head;
    CounterDelta* counter_delta_head;
    struct Transaction* next;
} Transaction;

BalanceDelta* balance_delta_alloc(CurrencyValue* cv, Person* target);
BalanceDelta* balance_delta_add(BalanceDelta** head, BalanceDelta* delta);
void balance_delta_free(BalanceDelta* delta);
void balance_deltas_free(BalanceDelta* head);

PacksDelta* packs_delta_alloc(int packs);
PacksDelta* packs_delta_add(PacksDelta** head, PacksDelta* delta);
void packs_delta_free(PacksDelta* delta);
void packs_deltas_free(PacksDelta* head);

CounterDelta* counter_delta_alloc(int counter, Person* target);
CounterDelta* counter_delta_add(CounterDelta** head, CounterDelta* delta);
void counter_delta_free(CounterDelta* delta);
void counter_deltas_free(CounterDelta* head);

Transaction* transaction_alloc(enum transaction_type type, long timestamp);
Transaction* transaction_add(Transaction** head, Transaction* t);
Transaction* transaction_pop(Transaction** head);
void transaction_free(Transaction* t);
void transactions_free(Transaction* head);

Transaction* transaction_invert(Transaction* transaction);
Transaction* clear_transactions_with_target(Transaction** head, Person* target);

#endif