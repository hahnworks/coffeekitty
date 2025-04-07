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
 
#include "person.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "currency.h"

Person* create_person_full(char* name, int balance, Currency* currency, float thirst, int current_coffees, int total_coffees)
{
    Person* p = malloc(sizeof(Person));
    p->name_length = strlen(name);
    p->name = malloc(p->name_length + 1);
    strcpy(p->name, name);

    p->balance = create_currency_value(balance, currency);

    p->thirst = thirst;
    p->current_coffees = current_coffees;
    p->total_coffees = total_coffees;

    p->next = NULL;
    
    return p;
}

Person* create_person(char* name, int balance, Currency* currency)
{
    return create_person_full(name, balance, currency, 0., 0, 0);
}

void free_person(Person *p)
{
    free(p->name);
    free_currency_value(p->balance);

    free(p);
}

void free_persons(Person *persons)
{
    Person *p = persons;
    while (p) {
        Person *next = p->next;
        free_person(p);
        p = next;
    }
}

Person* add_person(Person **head, Person *person)
{
    if (get_person_by_name(*head, person->name) != NULL) {
        return NULL;
    }

    // find end
    Person* end;
    for (end = *head; end && end->next; end = end->next);

    if (end == NULL) {
        *head = person;
    } else {
        end->next = person;
    }

    return person;
}

void remove_person(Person **head, Person *person_to_remove)
{
    Person *p = *head;
    Person *prev = NULL;
    while (p != NULL) {
        if (p == person_to_remove) {
            if (prev == NULL) { // p == *head
                *head = p->next;
            } else {
                prev->next = p->next;
            }
            person_to_remove->next = NULL;
            return;
        }
        // move forward
        prev = p;
        p = p->next;
    }

    return;
}

Person* rename_person(Person* persons, Person *person, char *new_name)
{
    if (get_person_by_name(persons, new_name)){
        return NULL;
    }

    free(person->name);
    person->name_length = strlen(new_name);
    person->name = malloc(person->name_length + 1);
    strcpy(person->name, new_name);

    return person;
}

Person *get_person_by_name(Person *persons, char *name)
{
    const Person *p = persons;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

void sort_persons_by_name(Person **old_head)
{
    Person* new_head = NULL;

    while (*old_head) {
        // find first
        Person* first = *old_head;
        for (Person* p = *old_head; p; p = p->next) {
            if (strcmp(p->name, first->name) <= 0) {
                first = p;
            }
        }

        remove_person(old_head, first);
        add_person(&new_head, first);
    }

    *old_head = new_head;    
    return;
}

int get_person_count(Person *head)
{
    int count = 0;
    for (Person *p = head; p; p = p->next) {
        count++;
    }
    return count;
}