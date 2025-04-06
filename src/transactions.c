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

#include "transactions.h"

#include <stdlib.h>

#include "person.h"
#include "currency.h"
#include "kitty.h"
#include "output.h"

BalanceDelta* create_balance_delta(CurrencyValue* cv, Person* target)
{
    BalanceDelta* delta = malloc(sizeof(BalanceDelta));
    delta->cv = cv;
    delta->target = target;
    delta->next = NULL;
    return delta;
}

BalanceDelta* add_balance_delta(BalanceDelta** head, BalanceDelta* delta)
{
    BalanceDelta* end;
    for (end = *head; end && end->next; end = end->next);

    if (end == NULL) {
        *head = delta;
    } else {
        end->next = delta;
    }

    return delta;
}

void free_balance_delta(BalanceDelta* delta)
{
    free_currency_value(delta->cv);
    free(delta);
}

void free_balance_deltas(BalanceDelta* head)
{
    BalanceDelta* bd = head;
    while (bd) {
        BalanceDelta* next = bd->next;
        free_balance_delta(bd);
        bd = next;
    }
}

PacksDelta* create_packs_delta(int packs)
{
    PacksDelta* delta = malloc(sizeof(PacksDelta));
    delta->packs = packs;
    delta->next = NULL;
    return delta;
}

PacksDelta* add_packs_delta(PacksDelta** head, PacksDelta* delta)
{
    PacksDelta* end;
    for (end = *head; end && end->next; end = end->next);

    if (end == NULL) {
        *head = delta;
    } else {
        end->next = delta;
    }

    return delta;
}

void free_packs_delta(PacksDelta* delta)
{
    free(delta);
}

void free_packs_deltas(PacksDelta* head)
{
    PacksDelta* pd = head;
    while (pd) {
        PacksDelta* next = pd->next;
        free_packs_delta(pd);
        pd = next;
    }
}

CounterDelta* create_counter_delta(int counter, Person* target)
{
    CounterDelta* delta = malloc(sizeof(CounterDelta));
    delta->counter = counter;
    delta->target = target;
    delta->next = NULL;
    return delta;
}

CounterDelta* add_counter_delta(CounterDelta** head, CounterDelta* delta)
{
    CounterDelta* end;
    for (end = *head; end && end->next; end = end->next);

    if (end == NULL) {
        *head = delta;
    } else {
        end->next = delta;
    }

    return delta;
}

void free_counter_delta(CounterDelta* delta)
{
    free(delta);
}

void free_counter_deltas(CounterDelta* head)
{
    CounterDelta* cd = head;
    while (cd) {
        CounterDelta* next = cd->next;
        free_counter_delta(cd);
        cd = next;
    }
}

Transaction* create_transaction(enum transaction_type type)
{
    Transaction* t = malloc(sizeof(Transaction));
    t->type = type;
    t->balance_delta_head = NULL;
    t->counter_delta_head = NULL;
    t->packs_delta_head = NULL;
    t->next = NULL;
    return t;
}

Transaction* add_transaction(Transaction** head, Transaction* transaction)
{
    Transaction* end;
    for (end = *head; end && end->next; end = end->next);

    if (end == NULL) {
        *head = transaction;
    } else {
        end->next = transaction;
    }

    return transaction;
}

void free_transaction(Transaction* t)
{
    free_balance_deltas(t->balance_delta_head);
    free(t);
}

void free_transactions(Transaction* head)
{
    Transaction* t = head;
    while (t) {
        Transaction* next = t->next;
        free_transaction(t);
        t = next;
    }
}