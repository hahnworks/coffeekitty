/*
 * This file is part of Coffeekitty
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

#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kitty.h"
#include "person.h"
#include "currency.h"
#include "settings.h"
#include "metainfo.h"

#include "output.h"
#include "colors.h"
#include "operations.h"
#include "latex.h"
#include "transactions.h"

const Command commands[] = {
    {"#", NULL, "General:"},
    {"print", command_print, "Print table (default)"},
    {"help", command_help, "Print help"},
    {"about", command_about, "Print about/version information"},
    {"version", command_about, "Print about/version information"},

    {"#", NULL, "Kitty management:"},
    {"set", command_set, "Set various settings"},

    {"#", NULL, "  Transaction management:"},
    {"drink", command_drink, "Drink coffee"},
    {"buy", command_buy, "Buy coffee"},
    {"pay", command_pay, "(Person) Pay(s) debt"},
    {"reimbursement", command_reimbursement, "(Person) Buy(s) something for the kitty"},
    {"consume", command_consume, "Consume a pack"},
    {"undo", command_undo, "Undo last transaction"},

    {"#", NULL, "  Output management:"},
    {"latex", command_latex, "Print latex sheet"},
    {"thirst", command_thirst, "Calculate thirst"},

    {"#", NULL, "  Person management:"},
    {"add", command_add, "Add a person"},
    {"remove", command_remove, "Remove a person"},
    {"rename", command_rename, "Rename a person"},

    {NULL, NULL, NULL}
};

void print_commands(char* argv0)
{
    printf("Usage: %s <operation> [options...]\n", argv0);
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

int command_about(int argc, char** argv, Kitty* kitty)
{
    (void)argc;
    (void)argv;
    (void)kitty;

    printf("\n%s%s - %s%s\n", ANSI_YELLOW, APPLICATION_NAME, APPLICATION_DESCRIPTION, ANSI_RESET);
    printf("Version v%s\n", APPLICATION_VERSION);
    printf("%s\n", COPYRIGHT);
    printf("\n");
    printf("%s\n\n", LICENSE);

    return 0;
}

/* Kitty management */

// this has to be rehauled
int command_set(int argc, char** argv, Kitty* kitty)
{
    if (argc < 3 || argc > 4) {
        printf("Usage: %s %s <option> <value>\n", argv[0], argv[1]);
        printf("Available options are:\n\tprice\n\tbalance\n\tpacks\n\tcurrency\n\tprefix_currency_value\n");
        return 1;
    }

    if (strcmp(argv[2], "price") == 0) {
        if (argc < 4) {
            printf("Usage: %s %s price <value>\n", argv[0], argv[1]);
            return 1;
        }
        Currency* currency = kitty->settings->currency;
        CurrencyValue* price = ftocv(atof(argv[3]), currency);
        currency_value_free(kitty->price);
        kitty->price = price;
        printf("Price set to %s\n", currency_value_format(price, false, true));
    } else if (strcmp(argv[2], "balance") == 0) {
        if (argc < 4) {
            printf("Usage: %s %s balance <value>\n", argv[0], argv[1]);
            return 1;
        }
        Currency* currency = kitty->settings->currency;
        CurrencyValue* balance = ftocv(atof(argv[3]), currency);
        currency_value_free(kitty->balance);
        kitty->balance = balance;
        printf("Balance set to %s\n", currency_value_format(balance, true, true));
    } else if (strcmp(argv[2], "packs") == 0) {
        if (argc < 4) {
            printf("Usage: %s %s packs <value>\n", argv[0], argv[1]);
            return 1;
        }
        kitty->packs = atoi(argv[3]);
        printf("Packs set to %i\n", kitty->packs);
    } else if (strcmp(argv[2], "currency") == 0) {
        if (argc < 4) {
            printf("Usage: %s %s currency <currency>\n", argv[0], argv[1]);
            return 1;
        }
        char* isoname = kitty->settings->currency->isoname;
        snprintf(isoname, 4, "%s", argv[3]);

        printf("Currency set to %s\n", isoname);
    } else if (strcmp(argv[2], "prefix_currency_value") == 0) {
        if (argc < 4) {
            printf("Usage: %s %s prefix_currency_value <true|false>\n", argv[0], argv[1]);
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
    if (argc != 4) {
        printf("Usage: %s %s <name> <amount>\n", argv[0], argv[1]);
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
    if (argc != 4) {
        printf("Usage: %s %s <amount> <cost>\n", argv[0], argv[1]);
        return 1;
    }

    int amount = atoi(argv[2]);
    Currency* currency = kitty->settings->currency;
    CurrencyValue* cost = ftocv(atof(argv[3]), currency);

    buy_coffee(kitty, amount, cost);

    currency_value_free(cost);

    return 0;
}

int command_pay(int argc, char** argv, Kitty* kitty)
{
    if (argc != 4) {
        printf("Usage: %s %s <name> <amount>\n", argv[0], argv[1]);
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

    currency_value_free(payment);

    return 0;
}

int command_reimbursement(int argc, char** argv, Kitty* kitty)
{
    if (argc != 4) {
        printf("Usage: %s %s <name> <amount>\n", argv[0], argv[1]);
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
    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], argv[1]);
        return 1;
    }

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
    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], argv[1]);
        return 1;
    }

    return fprint_new_latex_sheet(stdout, kitty);
}

int command_thirst(int argc, char** argv, Kitty* kitty)
{
    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], argv[1]);
        return 1;
    }

    calculate_thirst(kitty->persons);

    printf("Thirst calculated. Current coffees reset.\n");
    return 0;
}

int command_undo(int argc, char** argv, Kitty* kitty)
{
    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], argv[1]);
        return 1;
    }

    if (!kitty->transactions) {
        printf("No transactions to undo.\n");
        return 1;
    }
    
    Transaction* last_transaction;
    for (last_transaction = kitty->transactions; last_transaction->next; last_transaction = last_transaction->next);

    revert_transaction(kitty, last_transaction);
    printf("Last transaction undone.\n");
    return 0;
}

/* Person management */

int command_add(int argc, char** argv, Kitty* kitty)
{
    if (argc < 3) {
        printf("Usage: %s %s <names>...\n", argv[0], argv[1]);
        return 1;
    }

    //          kitty add person1 person2
    // argv[i]: i=0    1       2      3
    // argc:    1      2       3      4

    for (int i=2; i<argc; i++) {
        Person *new_person = create_person(argv[i], 0, kitty->settings->currency);
        if (person_add(&kitty->persons, new_person)) {
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
        printf("Usage: %s %s <names>...\n", argv[0], argv[1]);
        return 1;
    }

    for (int i=2; i<argc; i++) {
        Person* person_to_remove = get_person_by_name(kitty->persons, argv[i]);
        if (!person_to_remove) {
            printf("Person to remove %s not found\n", argv[2]);
            return 1;
        }

        if (person_to_remove->balance->value != 0) {
            printf("%sPerson %s has a non-zero balance!%s\n", ANSI_RED, person_to_remove->name, ANSI_RESET);
        }

        printf("Are you sure you want to remove %s?\n"
               "This will remove all transactions related to this person.\n"
               "THIS ACTION CANNOT BE UNDONE. (y/N): ", person_to_remove->name);
        char answer = getchar();
        while (getchar() != '\n'); // clear input buffer
        if (answer != 'y' && answer != 'Y') {
            printf("Aborting removal of %s\n", person_to_remove->name);
            continue;
        }

        person_remove(&kitty->persons, person_to_remove);
        clear_transactions_with_target(&kitty->transactions, person_to_remove);
        printf("Sucessfully removed person %s\n", person_to_remove->name);
        person_free(person_to_remove);
    }

    return 0;
}

int command_rename(int argc, char** argv, Kitty* kitty)
{
    if (argc != 4) {
        printf("Usage: %s %s <old_name> <new_name>\n", argv[0], argv[1]);
        return 1;
    }

    Person* person_to_rename = get_person_by_name(kitty->persons, argv[2]);
    if (!person_to_rename) {
        printf("Person to rename %s not found\n", argv[2]);
        return 1;
    }

    if (person_rename( kitty->persons, person_to_rename, argv[3]))
        printf("Sucessfully renamed person %s to %s\n", argv[2], argv[3]);
    else
        printf("Failed to rename person %s to %s\n", argv[2], argv[3]);


    return 0;
}