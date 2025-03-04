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

void fprint_output(FILE* file, Kitty* kitty)
{
    // Determine widths
    int name_width = strlen("Name");
    int balance_width = strlen("Balance");
    int current_counter_width = strlen("Counter");
    int total_counter_width = strlen("Total");
    int thirst_width = strlen("Thirst");
    for (Person* p = kitty->persons; p; p = p->next) {
        int this_name_width = strlen(p->name);
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
            name_width, p->name,
            format_currency_value_color_prefix(p->balance),
            balance_width, format_currency_value(p->balance, false, true),
            format_currency_value_color_suffix(p->balance),
            current_counter_width, p->current_coffees,
            total_counter_width, p->total_coffees, 
            thirst_width, p->thirst);
    }
}


