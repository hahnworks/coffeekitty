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
#include <string.h>
#include <time.h>

#include "person.h"
#include "currency.h"

BalanceDelta* balance_delta_alloc(CurrencyValue* cv, Person* target)
{
    BalanceDelta* delta = malloc(sizeof(BalanceDelta));
    delta->cv = cv;
    delta->target = target;
    delta->next = NULL;
    return delta;
}

BalanceDelta* balance_delta_add(BalanceDelta** head, BalanceDelta* delta)
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

void balance_delta_free(BalanceDelta* delta)
{
    currency_value_free(delta->cv);
    free(delta);
}

void balance_deltas_free(BalanceDelta* head)
{
    BalanceDelta* bd = head;
    while (bd) {
        BalanceDelta* next = bd->next;
        balance_delta_free(bd);
        bd = next;
    }
}

PacksDelta* packs_delta_alloc(int packs)
{
    PacksDelta* delta = malloc(sizeof(PacksDelta));
    delta->packs = packs;
    delta->next = NULL;
    return delta;
}

PacksDelta* packs_delta_add(PacksDelta** head, PacksDelta* delta)
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

void packs_delta_free(PacksDelta* delta)
{
    free(delta);
}

void packs_deltas_free(PacksDelta* head)
{
    PacksDelta* pd = head;
    while (pd) {
        PacksDelta* next = pd->next;
        packs_delta_free(pd);
        pd = next;
    }
}

CounterDelta* counter_delta_alloc(int counter, Person* target)
{
    CounterDelta* delta = malloc(sizeof(CounterDelta));
    delta->counter = counter;
    delta->target = target;
    delta->next = NULL;
    return delta;
}

CounterDelta* counter_delta_add(CounterDelta** head, CounterDelta* delta)
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

void counter_delta_free(CounterDelta* delta)
{
    free(delta);
}

void counter_deltas_free(CounterDelta* head)
{
    CounterDelta* cd = head;
    while (cd) {
        CounterDelta* next = cd->next;
        counter_delta_free(cd);
        cd = next;
    }
}

Transaction* transaction_alloc(enum transaction_type type, long timestamp)
{
    Transaction* t = malloc(sizeof(Transaction));
    t->type = type;
    t->timestamp = timestamp;
    t->balance_delta_head = NULL;
    t->counter_delta_head = NULL;
    t->packs_delta_head = NULL;
    t->next = NULL;

    if (timestamp == -1)
        t->timestamp = time(NULL);

    return t;
}

Transaction* transaction_add(Transaction** head, Transaction* transaction)
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

Transaction* transaction_pop(Transaction** head)
{
    if (!*head) // no transactions
        return NULL;

    if (!(*head)->next) { // only one transaction
        Transaction* t = *head;
        *head = NULL;
        return t;
    }

    Transaction *lt;
    Transaction *oblt;

    oblt = *head;
    lt = oblt->next;
    while (lt->next) { // while lt is not last (otherwise lt->next == NULL)
        oblt = oblt->next;
        lt = oblt->next;
    }

    oblt->next = NULL;
    return lt;
}

Transaction* transaction_remove(Transaction** head, Transaction* transaction)
{
    if (!*head) // no transactions
        return NULL;

    if (*head == transaction) { // first transaction
        Transaction* t = *head;
        *head = (*head)->next;
        return t;
    }

    // all other cases
    Transaction* prev = *head;
    while (prev->next && prev->next != transaction) { // shift until found and end is not reached
        prev = prev->next;
    }

    if (prev->next == NULL) // transaction not found
        return NULL;

    Transaction* t = prev->next; // pick
    prev->next = t->next; // reconnect
    return t;
}

void transaction_free(Transaction* t)
{
    balance_deltas_free(t->balance_delta_head);
    counter_deltas_free(t->counter_delta_head);
    packs_deltas_free(t->packs_delta_head);
    free(t);
}

void transactions_free(Transaction* head)
{
    Transaction* t = head;
    while (t) {
        Transaction* next = t->next;
        transaction_free(t);
        t = next;
    }
}

Transaction* transaction_invert(Transaction* transaction)
{
    Transaction* inverted = transaction_alloc(UNDO, -1);

    for (BalanceDelta* bd = transaction->balance_delta_head; bd; bd = bd->next) {
        CurrencyValue* cv = currency_value_new_negative(bd->cv);
        balance_delta_add(&inverted->balance_delta_head, balance_delta_alloc(cv, bd->target));
    }
    for (PacksDelta* pd = transaction->packs_delta_head; pd; pd = pd->next) {
        packs_delta_add(&inverted->packs_delta_head, packs_delta_alloc(-pd->packs));
    }
    for (CounterDelta* cd = transaction->counter_delta_head; cd; cd = cd->next) {
        counter_delta_add(&inverted->counter_delta_head, counter_delta_alloc(-cd->counter, cd->target));
    }

    return inverted;
}

Transaction* clear_transactions_with_target(Transaction** head, Person* target)
{
    for(Transaction* t = *head; t; t = t->next) {
        if (
            (t->balance_delta_head && t->balance_delta_head->target == target)
            ||
            (t->counter_delta_head && t->counter_delta_head->target == target)
        ) {
            transaction_remove(head, t);
        }
    }

    return *head;
}