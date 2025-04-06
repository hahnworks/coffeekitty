/*
 * This file is part of Coffeekitty
 * 
 * Copyright (C) 2025 Alexander Hahn Hahn
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

#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kitty.h"
#include "person.h"
#include "currency.h"
#include "settings.h"

#include "output.h"
#include "operations.h"
#include "latex.h"

const Command commands[] = {
    {"#", NULL, "General:"},
    {"print", command_print, "Print table (default)"},
    {"help", command_help, "Print help"},

    {"#", NULL, "Kitty management:"},
    {"set", command_set, "Set various settings"},

    {"#", NULL, "  Transaction management:"},
    {"drink", command_drink, "Drink coffee"},
    {"buy", command_buy, "Buy coffee"},
    {"pay", command_pay, "(Person) Pay(s) debt"},
    {"reimbursement", command_reimbursement, "(Person) Buy(s) something for the kitty"},
    {"consume", command_consume, "Consume a pack"},

    {"#", NULL, "  Output management:"},
    {"latex", command_latex, "Print latex sheet"},
    {"thirst", command_thirst, "Calculate thirst"},

    {"#", NULL, "  Person management:"},
    {"add", command_add, "Add a person"},
    {"remove", command_remove, "Remove a person"},

    {NULL, NULL, NULL}
};

void print_commands(char* argv0)
{
    printf("Usage: %s [operation] <options...>\n", argv0);
    printf("Operations:\n");
    for (int cptr = 0; commands[cptr].name; cptr++) {
        const Command* c = &commands[cptr];

        if (!c->function) {
            printf("\n%s\n", c->help);
            continue;
        }

        printf("\t%s: %s\n", c->name, c->help);
    }
}

int parse_command(int argc, char** argv, Kitty* kitty)
{
    if (argc < 2) {
        return commands[1].function(argc, argv, kitty);
    }

    for (int cptr = 0; commands[cptr].name; cptr++) {
        const Command* c = &commands[cptr];

        if (!c->function)
            continue;

        if (strcmp(argv[1], c->name) == 0) {
            return c->function(argc, argv, kitty);
        }
    }

    printf("Command %s not found.\n Try %s help\n", argv[1], argv[0]);
    return 1;
}

/* General */

int command_print(int argc, char** argv, Kitty* kitty)
{
    if (argc > 2) {
        Person* p = get_person_by_name(kitty->persons, argv[2]);
        if (p) {
            fprint_person(stdout, p);
        } else {
            printf("Person not found\n");
        }
    }
    else { // no additional argument given
        fprint_output(stdout, kitty);
    }

    return 0;
}

int command_help(int argc, char** argv, Kitty* kitty)
{
    (void)argc;
    (void)kitty;
    print_commands(argv[0]);
    return 1;
}

/* Kitty management */

int command_set(int argc, char** argv, Kitty* kitty)
{
    if (argc < 3) {
        printf("Usage: %s set <option> <value>\n", argv[0]);
        printf("Available options are:\n\tprice\n\tbalance\n\tpacks\n\tcurrency\n\tprefix_currency_value\n");
        return 1;
    }

    if (strcmp(argv[2], "price") == 0) {
        if (argc < 4) {
            printf("Usage: %s set price <value>\n", argv[0]);
            return 1;
        }
        Currency* currency = kitty->settings->currency;
        CurrencyValue* price = ftocv(atof(argv[3]), currency);
        free_currency_value(kitty->price);
        kitty->price = price;
        printf("Price set to %s\n", format_currency_value(price, false, true));
    } else if (strcmp(argv[2], "balance") == 0) {
        if (argc < 4) {
            printf("Usage: %s set balance <value>\n", argv[0]);
            return 1;
        }
        Currency* currency = kitty->settings->currency;
        CurrencyValue* balance = ftocv(atof(argv[3]), currency);
        free_currency_value(kitty->balance);
        kitty->balance = balance;
        printf("Balance set to %s\n", format_currency_value(balance, true, true));
    } else if (strcmp(argv[2], "packs") == 0) {
        if (argc < 4) {
            printf("Usage: %s set packs <value>\n", argv[0]);
            return 1;
        }
        kitty->packs = atoi(argv[3]);
        printf("Packs set to %i\n", kitty->packs);
    } else if (strcmp(argv[2], "currency") == 0) {
        if (argc < 4) {
            printf("Usage: %s set currency <currency>\n", argv[0]);
            return 1;
        }
        char* isoname = kitty->settings->currency->isoname;
        snprintf(isoname, 4, "%s", argv[3]);

        printf("Currency set to %s\n", isoname);
    } else if (strcmp(argv[2], "prefix_currency_value") == 0) {
        if (argc < 4) {
            printf("Usage: %s set prefix_currency_value <true|false>\n", argv[0]);
            return 1;
        }

        if (strcmp(argv[3], "true") == 0 || argv[3][0] == '1') {
            kitty->settings->currency->prefix = true;
        } else if (strcmp(argv[3], "false") == 0 || argv[3][0] == '0') {
            kitty->settings->currency->prefix = false;
        } else {
            printf("Unknown value %s\n", argv[3]);
            return 1;
        }

        printf("Prefix currency value set to %s\n", kitty->settings->currency->prefix ? "true" : "false");
    } else {
        printf("Unknown setting %s\n", argv[2]);
        return 1;
    }

    return 0;
}

/* Transaction management */

int command_drink(int argc, char** argv, Kitty* kitty)
{
    if (argc < 4) {
        printf("Usage: %s drink <name> <amount>\n", argv[0]);
        return 1;
    }
    Person *p = get_person_by_name(kitty->persons, argv[2]);
    if (!p) {
        printf("Person %s not found\n", argv[2]);
        return 1;
    }

    int amount = atoi(argv[3]);
    person_drinks_coffee(kitty, p, amount);
    return 0;
}

int command_buy(int argc, char** argv, Kitty* kitty)
{
    if (argc < 3) {
        printf("Usage: %s buy <amount> <cost>\n", argv[0]);
        return 1;
    }

    int amount = atoi(argv[2]);
    Currency* currency = kitty->settings->currency;
    CurrencyValue* cost = ftocv(atof(argv[3]), currency);

    buy_coffee(kitty, amount, cost);

    free_currency_value(cost);

    return 0;
}

int command_pay(int argc, char** argv, Kitty* kitty)
{
    if (argc < 4) {
        printf("Usage: %s pay <name> <amount>\n", argv[0]);
        return 1;
    }
    Person *p = get_person_by_name(kitty->persons, argv[2]);
    if (!p) {
        printf("Person %s not found\n", argv[2]);
        return 1;
    }

    Currency* currency = kitty->settings->currency;
    CurrencyValue* payment = ftocv(atof(argv[3]), currency);

    person_pays_debt(kitty,p, payment);

    free_currency_value(payment);

    return 0;
}

int command_reimbursement(int argc, char** argv, Kitty* kitty)
{
    if (argc < 3) {
        printf("Usage: %s auslage <name> <amount>\n", argv[0]);
        return 1;
    }
    Person *p = get_person_by_name(kitty->persons, argv[2]);
    if (!p) {
        printf("Person %s not found\n", argv[2]);
        return 1;
    }

    Currency* currency = kitty->settings->currency;
    CurrencyValue* cost = ftocv(atof(argv[3]), currency);
    
    person_buys_misc(kitty, p, cost);

    return 0;
}

int command_consume(int argc, char** argv, Kitty* kitty)
{
    (void)argc;
    (void)argv;

    if (kitty->packs == 0) {
        printf("No packs left\n");
        return 1;
    }
    consume_pack(kitty);
    return 0;
}

/* Output */

int command_latex(int argc, char** argv, Kitty* kitty)
{
    (void)argc;
    (void)argv;
    return fprint_new_latex_sheet(stdout, kitty);
}

int command_thirst(int argc, char** argv, Kitty* kitty)
{
    (void)argc;
    (void)argv;
    calculate_thirst(kitty->persons);

    printf("Thirst calculated. Current coffees reset.\n");
    return 0;
}


/* Person management */

int command_add(int argc, char** argv, Kitty* kitty)
{
    if (argc < 3) {
        printf("Usage: %s add <name>\n", argv[0]);
        return 1;
    }

    //          kitty add person1 person2
    // argv[i]: i=0    1       2      3
    // argc:    1      2       3      4

    for (int i=2; i<argc; i++) {
        Person *new_person = create_person(argv[i], 0, kitty->settings->currency);
        if (add_person(&kitty->persons, new_person)) {
            printf("Sucessfully added person %s\n", new_person->name);
        } else {
            printf("Failed to add person %s\n", new_person->name);
            return 1;
        }
    }
    return 0;
}

int command_remove(int argc, char** argv, Kitty* kitty)
{
    if (argc < 3) {
        printf("Usage: %s remove <name>\n", argv[0]);
        return 1;
    }

    for (int i=2; i<argc; i++) {
        Person* person_to_remove = get_person_by_name(kitty->persons, argv[i]);
        if (!person_to_remove) {
            printf("Person to remove %s not found\n", argv[2]);
            return 1;
        }
        remove_person(&kitty->persons, person_to_remove);
        printf("Sucessfully removed person %s\n", person_to_remove->name);
    }

    return 0;
}