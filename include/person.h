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

#ifndef PERSON_H
#define PERSON_H

#include "currency.h"

typedef struct Person{
    char *name;
    int name_length;

    float thirst;
    int current_coffees;
    int total_coffees;
    CurrencyValue* balance;

    struct Person* next;
} Person;

Person* person_create_full(char* name, int balance, Currency* currency, float thirst, int current_coffees, int total_coffees);
Person* create_person(char* name, int balance, Currency* currency);
void person_free(Person *p);
void persons_free(Person *persons);
Person* person_add(Person **head, Person *person);
void person_remove(Person **head, Person *person_to_remove);
Person* person_rename(Person *persons, Person *person, char *new_name);
Person *get_person_by_name(Person *persons, char *name);
void sort_persons_by_name(Person **head);
int get_person_count(Person *persons);

#endif