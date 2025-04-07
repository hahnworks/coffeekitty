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

#ifndef COMMANDS_H
#define COMMANDS_H 

#include "kitty.h"

typedef struct Command {
    char* name;
    int (*function)(int argc, char** argv, Kitty* kitty);
    char* help;
} Command;

void print_commands(char* argv0);

// General
int command_print(int argc, char** argv, Kitty* kitty);
int command_help(int argc, char** argv, Kitty* kitty);
// Kitty management
int command_set(int argc, char** argv, Kitty* kitty);
// Transaction management
int command_drink(int argc, char** argv, Kitty* kitty);
int command_buy(int argc, char** argv, Kitty* kitty);
int command_pay(int argc, char** argv, Kitty* kitty);
int command_reimbursement(int argc, char** argv, Kitty* kitty);
int command_consume(int argc, char** argv, Kitty* kitty);
int command_undo(int argc, char** argv, Kitty* kitty);
// Output management
int command_latex(int argc, char** argv, Kitty* kitty);
int command_thirst(int argc, char** argv, Kitty* kitty);
// Person management
int command_add(int argc, char** argv, Kitty* kitty);
int command_remove(int argc, char** argv, Kitty* kitty);
int command_rename(int argc, char** argv, Kitty* kitty);


int parse_command(int argc, char** argv, Kitty* kitty);

#endif