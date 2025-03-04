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

#include "person.h"
#include "currency.h"
#include "kitty.h"

void person_pays_debt(Person* person, Kitty* kitty, CurrencyValue* payment)
{
    add_to_currency_value(person->balance, payment);
    add_to_currency_value(kitty->balance, payment);
}

void person_buys_misc(Person* person, CurrencyValue* cost)
{
    add_to_currency_value(person->balance, cost);
}

void person_drinks_coffee(Person* person, Kitty* kitty, int amount)
{
    kitty->counter += amount;

    for (int i=0; i<amount; i++)
        sub_from_currency_value(person->balance, kitty->price);

    person->current_coffees += amount;
    person->total_coffees += amount;
}

void buy_coffee(Kitty* kitty, int amount, CurrencyValue* cost)
{
    kitty->packs += amount;
    sub_from_currency_value(kitty->balance, cost);
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
    kitty->packs--;
}