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
    Transaction* t = create_transaction(PERSON_PAYS_DEBT);
    add_balance_delta(&t->balance_delta_head, create_balance_delta(copy_currency_value(payment), person));
    add_balance_delta(&t->balance_delta_head, create_balance_delta(copy_currency_value(payment), NULL));

    apply_transaction(kitty, t);
    add_transaction(&kitty->transactions, t);
}

void person_buys_misc(Kitty* kitty, Person* person, CurrencyValue* cost)
{
    Transaction* t = create_transaction(PERSON_BUYS_MISC);
    add_balance_delta(&t->balance_delta_head, create_balance_delta(copy_currency_value(cost), person));

    apply_transaction(kitty, t);
    add_transaction(&kitty->transactions, t);
}

void person_drinks_coffee(Kitty* kitty, Person* person, int amount)
{
    Transaction* t = create_transaction(PERSON_DRINKS_COFFEE);

    CounterDelta* cd_p = create_counter_delta(amount, person);    
    CounterDelta* cd_k = create_counter_delta(amount, NULL);    
    add_counter_delta(&t->counter_delta_head, cd_p);
    add_counter_delta(&t->counter_delta_head, cd_k);

    CurrencyValue* delta_cv = new_negative_currency_value(kitty->price);
    mul_currency_value(delta_cv, amount);
    add_balance_delta(&t->balance_delta_head, create_balance_delta(delta_cv, person));

    apply_transaction(kitty, t);
    add_transaction(&kitty->transactions, t);
}

void buy_coffee(Kitty* kitty, int amount, CurrencyValue* cost)
{
    Transaction* t = create_transaction(KITTY_BUY_COFFEE);
    PacksDelta* pd = create_packs_delta(amount);
    add_packs_delta(&t->packs_delta_head, pd);

    CurrencyValue* delta_cv = new_negative_currency_value(cost);
    add_balance_delta(&t->balance_delta_head, create_balance_delta(delta_cv, NULL));

    apply_transaction(kitty, t);
    add_transaction(&kitty->transactions, t);
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
    Transaction* t = create_transaction(KITTY_CONSUME_PACK);
    PacksDelta* pd = create_packs_delta(-1);
    add_packs_delta(&t->packs_delta_head, pd);

    apply_transaction(kitty, t);
    add_transaction(&kitty->transactions, t);
}

void apply_transaction(Kitty* k, Transaction* t){
    fprint_transaction(stdout, k, t, false);

    for(BalanceDelta* bd = t->balance_delta_head; bd; bd = bd->next) {
        if (bd->target) {
            add_to_currency_value(bd->target->balance, bd->cv);
        } else { // target is NULL ⇒ apply to kitty
            add_to_currency_value(k->balance, bd->cv);
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
    Transaction* inverted_t = invert_transaction(t);
    apply_transaction(k, inverted_t);

    free_transaction(pop_transaction(&k->transactions));
}