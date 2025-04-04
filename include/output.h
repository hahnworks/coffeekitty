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

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>

#include "kitty.h"
#include "person.h"
#include "transactions.h"

void fprint_person(FILE* file, Person *p);
void print_persons(Person *head);
void fprint_kitty(FILE* file, Kitty *k);
void print_help(char* argv0);
void fprint_transaction(FILE* file, Kitty* kitty, Transaction* transaction, bool verbose);

void fprint_hline(FILE* file, int width);
void fprint_output(FILE* file, Kitty* kitty);

#endif