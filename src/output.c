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

#include "output.h"

#include <stdio.h>
#include <string.h>

#include "kitty.h"
#include "person.h"
#include "colors.h"
#include "currency.h"
#include "transactions.h"

void fprint_person(FILE* file, Person *p)
{
    fprintf(file, "Name: %s\n", p->name);
    fprintf(file,"Balance: %s\n", format_currency_value(p->balance, true, true));
    fprintf(file, "\n");
    fprintf(file,"Thirst: %f\n", p->thirst);
    fprintf(file,"Current coffees: %i\n", p->current_coffees);
    fprintf(file,"Total: %i\n", p->total_coffees);
}

void print_persons(Person *head)
{
    Person *p = head;
    while (p) {
        printf("\n");
        fprint_person(stdout, p);
        p = p->next;
        printf("\n");
    }
}

void fprint_kitty(FILE* file, Kitty *kitty)
{
    fprintf(file, ANSI_YELLOW "+++ Kitty +++\n" ANSI_RESET);
    fprintf(file, "Balance: %s\n", format_currency_value(kitty->balance, true, true));
    fprintf(file, "Price: %s\n", format_currency_value(kitty->price, false, true));
    fprintf(file, "Packs: %i\n", kitty->packs);
    fprintf(file, "Counter: %i\n", kitty->counter);
}

void fprint_hline(FILE* file, int width)
{
    for (int i=0; i<width; i++) {
        fprintf(file, "-");
    }
    fprintf(file, "\n");
}

void fprint_transaction(FILE* file, Transaction* transaction){
    fprintf(file, ANSI_YELLOW "+++ Transaction +++\n" ANSI_RESET);

    fprintf(file, "Type: ");
    switch (transaction->type) {
    case PERSON_PAYS_DEBT:
        fprintf(file, "Person pays debt.\n");
        break;
    case PERSON_BUYS_MISC:
        fprintf(file, "Person buys misc.\n");
        break;
    case PERSON_DRINKS_COFFEE:
        fprintf(file, "Person drinks coffee.\n");
        break;
    case KITTY_BUY_COFFEE:
        fprintf(file, "Buying coffee.\n");
        break;
    case KITTY_CONSUME_PACK:
        fprintf(file, "A pack is consumed.\n");
        break;
    case UNDO:
        fprintf(file, "Inverted transaction.\n");
        break;
    default:
        fprintf(file, "Unknown transaction type.\n");
        break;
    }

    fprintf(file, "Changes: \n");
    if (transaction->balance_delta_head)
        fprintf(file, "-> Balance: \n");
    for (BalanceDelta* bd = transaction->balance_delta_head; bd; bd = bd->next) {
        if (bd->target)
            fprintf(file, "   Balance of %s is modified by ", bd->target->name);
        else
            fprintf(file, "   Kitty balance is modified by ");

        fprintf(file, "%s\n", format_currency_value(bd->cv, true, true));
    }

    if (transaction->packs_delta_head)
        fprintf(file, "-> Packs: \n");
    char verb[6] = {0};
    for (PacksDelta* pd = transaction->packs_delta_head; pd; pd = pd->next) {
        if (pd->packs > 0)
            fprintf(file, "   Kitty gets %i packs\n", pd->packs);
        else if (pd->packs < 0)
            fprintf(file, "   Kitty loses %i packs\n", -pd->packs);
    }

    if (transaction->counter_delta_head)
        fprintf(file, "-> Counters: \n");
    for (CounterDelta* cd = transaction->counter_delta_head; cd; cd = cd->next) {
        if (cd->counter >= 0)
            strcpy(verb, "gets");
        else if (cd->counter < 0)
            strcpy(verb, "loses");

        if (cd->target)
            fprintf(file, "   %s %s %i coffees\n", cd->target->name, verb, cd->counter);
        else
            fprintf(file, "   Total %s %i coffees\n", verb, cd->counter);
    }
}

int utf8_strlen(const char* s)
{
    int len = 0;
    for (int i = 0; s[i]; i++) {
        if ((s[i] & 0xC0) != 0x80) {
            len++;
        }
    }
    return len;
}

int excess_bytes(const char* str)
{
    return strlen(str) - utf8_strlen(str);
}

void fprint_output(FILE* file, Kitty* kitty)
{
    // Determine widths
    int name_width = strlen("Name");
    int balance_width = strlen("Balance");
    int current_counter_width = strlen("Counter");
    int total_counter_width = strlen("Total");
    int thirst_width = strlen("Thirst");
    for (Person* p = kitty->persons; p; p = p->next) {
        int this_name_width = utf8_strlen(p->name);
        int this_balance_width = strlen(format_currency_value(p->balance, false, true));
        int this_current_counter_width = snprintf(NULL, 0, "%i", p->current_coffees);
        int this_total_counter_width = snprintf(NULL, 0, "%i", p->total_coffees);
        int this_thirst_width = snprintf(NULL, 0, "%f", p->thirst);
        name_width = this_name_width > name_width ? this_name_width : name_width;
        balance_width = this_balance_width > balance_width ? this_balance_width : balance_width;
        current_counter_width = this_current_counter_width > current_counter_width ? this_current_counter_width : current_counter_width;
        total_counter_width = this_total_counter_width > total_counter_width ? this_total_counter_width : total_counter_width;
        thirst_width = this_thirst_width > thirst_width ? this_thirst_width : thirst_width;
    }
    int balance_width_w_color = balance_width + strlen(ANSI_RED) + strlen(ANSI_RESET);
    int total_width = name_width + balance_width_w_color + current_counter_width + total_counter_width + thirst_width + 9;

    // Print
    fprint_kitty(file, kitty);
    fprint_hline(file, total_width);
    fprintf(file, "%-*s | %-*s | %-*s / %-*s | %-*s\n", name_width, "Name", balance_width, "Balance", current_counter_width, "Counter", total_counter_width, "Total", thirst_width, "Thirst");
    fprint_hline(file, total_width);
    for (Person* p = kitty->persons; p; p = p->next) {
        fprintf(file, "%-*s | %s%*s%s | %*i / %*i | %-*f\n",
            name_width + excess_bytes(p->name), p->name,
            format_currency_value_color_prefix(p->balance),
            balance_width, format_currency_value(p->balance, false, true),
            format_currency_value_color_suffix(p->balance),
            current_counter_width, p->current_coffees,
            total_counter_width, p->total_coffees, 
            thirst_width, p->thirst);
    }
}


