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

BalanceDelta* create_balance_delta(CurrencyValue* cv, Person* target);
BalanceDelta* add_balance_delta(BalanceDelta** head, BalanceDelta* delta);
void free_balance_delta(BalanceDelta* delta);
void free_balance_deltas(BalanceDelta* head);

PacksDelta* create_packs_delta(int packs);
PacksDelta* add_packs_delta(PacksDelta** head, PacksDelta* delta);
void free_packs_delta(PacksDelta* delta);
void free_packs_deltas(PacksDelta* head);

CounterDelta* create_counter_delta(int counter, Person* target);
CounterDelta* add_counter_delta(CounterDelta** head, CounterDelta* delta);
void free_counter_delta(CounterDelta* delta);
void free_counter_deltas(CounterDelta* head);

Transaction* create_transaction(enum transaction_type type, long timestamp);
Transaction* add_transaction(Transaction** head, Transaction* t);
Transaction* pop_transaction(Transaction** head);
void free_transaction(Transaction* t);
void free_transactions(Transaction* head);

Transaction* invert_transaction(Transaction* transaction);
Transaction* clear_transactions_with_target(Transaction** head, Person* target);

#endif