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

#include "operations.h"

#include <stdlib.h>

#include "output.h"
#include "person.h"
#include "currency.h"
#include "kitty.h"
#include "transactions.h"

void person_pays_debt(Kitty* kitty, Person* person, CurrencyValue* payment)
{
    Transaction* t = transaction_alloc(PERSON_PAYS_DEBT, -1);
    balance_delta_add(&t->balance_delta_head, balance_delta_alloc(currency_value_copy(payment), person));
    balance_delta_add(&t->balance_delta_head, balance_delta_alloc(currency_value_copy(payment), NULL));

    apply_transaction(kitty, t);
    transaction_add(&kitty->transactions, t);
}

void person_buys_misc(Kitty* kitty, Person* person, CurrencyValue* cost)
{
    Transaction* t = transaction_alloc(PERSON_BUYS_MISC, -1);
    balance_delta_add(&t->balance_delta_head, balance_delta_alloc(currency_value_copy(cost), person));

    apply_transaction(kitty, t);
    transaction_add(&kitty->transactions, t);
}

void person_drinks_coffee(Kitty* kitty, Person* person, int amount)
{
    Transaction* t = transaction_alloc(PERSON_DRINKS_COFFEE, -1);

    CounterDelta* cd_p = counter_delta_alloc(amount, person);    
    CounterDelta* cd_k = counter_delta_alloc(amount, NULL);    
    counter_delta_add(&t->counter_delta_head, cd_p);
    counter_delta_add(&t->counter_delta_head, cd_k);

    CurrencyValue* delta_cv = currency_value_new_negative(kitty->price);
    currency_value_mul(delta_cv, amount);
    balance_delta_add(&t->balance_delta_head, balance_delta_alloc(delta_cv, person));

    apply_transaction(kitty, t);
    transaction_add(&kitty->transactions, t);
}

void buy_coffee(Kitty* kitty, int amount, CurrencyValue* cost)
{
    Transaction* t = transaction_alloc(KITTY_BUY_COFFEE, -1);
    PacksDelta* pd = packs_delta_alloc(amount);
    packs_delta_add(&t->packs_delta_head, pd);

    CurrencyValue* delta_cv = currency_value_new_negative(cost);
    balance_delta_add(&t->balance_delta_head, balance_delta_alloc(delta_cv, NULL));

    apply_transaction(kitty, t);
    transaction_add(&kitty->transactions, t);
}

void calculate_thirst(Person* persons)
{
    int current_total_coffees = 0;
    for (Person* p = persons; p; p = p->next)
        current_total_coffees += p->current_coffees;

    if (current_total_coffees != 0) {
        for (Person* p = persons; p; p = p->next) {
            p->thirst = (float) p->current_coffees / current_total_coffees;
            p->current_coffees = 0;
        }
    }
}

void consume_pack(Kitty* kitty)
{
    Transaction* t = transaction_alloc(KITTY_CONSUME_PACK, -1);
    PacksDelta* pd = packs_delta_alloc(-1);
    packs_delta_add(&t->packs_delta_head, pd);

    apply_transaction(kitty, t);
    transaction_add(&kitty->transactions, t);
}

void apply_transaction(Kitty* k, Transaction* t){
    fprint_transaction(stdout, t);

    for(BalanceDelta* bd = t->balance_delta_head; bd; bd = bd->next) {
        if (bd->target) {
            currency_value_add(bd->target->balance, bd->cv);
        } else { // target is NULL ⇒ apply to kitty
            currency_value_add(k->balance, bd->cv);
        }
    }
    for (PacksDelta* pd = t->packs_delta_head; pd; pd = pd->next) {
        k->packs += pd->packs;
    }
    for (CounterDelta* cd = t->counter_delta_head; cd; cd = cd->next) {
        if (cd->target) {
            cd->target->current_coffees += cd->counter;
            cd->target->total_coffees += cd->counter;
        } else { // target is NULL ⇒ apply to kitty
            k->counter += cd->counter;
        }
    }
}

void revert_transaction(Kitty* k, Transaction* t)
{
    Transaction* inverted_t = transaction_invert(t);
    apply_transaction(k, inverted_t);

    transaction_free(transaction_pop(&k->transactions));
}