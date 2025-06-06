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

#include "storage.h"

#include "kitty.h"
#include "settings.h"
#include "currency.h"
#include "person.h"
#include "transactions.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#if defined(__linux__)
    #include <linux/limits.h>
#elif defined(__APPLE__)
    #include <sys/syslimits.h>
#endif

CounterDelta* xml_parse_counter_delta(const xmlNode* counter_delta_node, Person* persons)
{
    xmlChar *target_name = xmlGetProp(counter_delta_node, (const xmlChar*) "target");
    xmlChar *value_string = xmlGetProp(counter_delta_node, (const xmlChar*) "value");

    CounterDelta* counter_delta = counter_delta_alloc(atoi((char*) value_string), NULL);
    if (target_name) {
        Person* target = get_person_by_name(persons, (char*) target_name);
        counter_delta->target = target;
    }

    xmlFree(target_name);
    xmlFree(value_string);

    return counter_delta;
}

CounterDelta* xml_parse_counter_deltas(const xmlNode* counter_deltas_node, Person* persons)
{
    CounterDelta *counter_deltas = NULL;
    for (xmlNode *delta_node = counter_deltas_node->children; delta_node; delta_node = delta_node->next) {
        if (delta_node->type == XML_ELEMENT_NODE && xmlStrcmp(delta_node->name, (const xmlChar*) "counter_delta") == 0) {
            CounterDelta* delta = xml_parse_counter_delta(delta_node, persons);
            counter_delta_add(&counter_deltas, delta);
        }
    }
    return counter_deltas;
}

PacksDelta* xml_parse_packs_delta(const xmlNode* packs_delta_node)
{
    xmlChar *value_string = xmlGetProp(packs_delta_node, (const xmlChar*) "value");
    PacksDelta* packs_delta = packs_delta_alloc(atoi((char*) value_string));

    xmlFree(value_string);

    return packs_delta;
}

PacksDelta* xml_parse_packs_deltas(const xmlNode* packs_deltas_node)
{
    PacksDelta *packs_deltas = NULL;
    for (xmlNode *delta_node = packs_deltas_node->children; delta_node; delta_node = delta_node->next) {
        if (delta_node->type == XML_ELEMENT_NODE && xmlStrcmp(delta_node->name, (const xmlChar*) "packs_delta") == 0) {
            PacksDelta* delta = xml_parse_packs_delta(delta_node);
            packs_delta_add(&packs_deltas, delta);
        }
    }
    return packs_deltas;
}

BalanceDelta* xml_parse_balance_delta(const xmlNode* balance_delta_node, Person* persons, const Settings* settings)
{
    xmlChar *target_name = xmlGetProp(balance_delta_node, (const xmlChar*) "target");
    xmlChar *value_string = xmlGetProp(balance_delta_node, (const xmlChar*) "value");

    BalanceDelta* balance_delta = balance_delta_alloc(currency_value_alloc(atoi((char*) value_string), settings->currency), NULL);
    if (target_name) {
        Person* target = get_person_by_name(persons, (char*) target_name);
        balance_delta->target = target;
    }

    xmlFree(target_name);
    xmlFree(value_string);

    return balance_delta;
}

BalanceDelta* xml_parse_balance_deltas(const xmlNode* balance_deltas_node, Person* persons, const Settings* settings)
{
    BalanceDelta *balance_deltas = NULL;
    for (xmlNode *delta_node = balance_deltas_node->children; delta_node; delta_node = delta_node->next) {
        if (delta_node->type == XML_ELEMENT_NODE && xmlStrcmp(delta_node->name, (const xmlChar*) "balance_delta") == 0) {
            BalanceDelta* delta = xml_parse_balance_delta(delta_node, persons, settings);
            balance_delta_add(&balance_deltas, delta);
        }
    }
    return balance_deltas;
}

Transaction* xml_parse_transaction(const xmlNode* transaction_node, Person* persons, const Settings* settings)
{
    xmlChar* type_string = xmlGetProp(transaction_node, (const xmlChar*) "type");
    enum transaction_type type = atoi((char*) type_string);
    xmlChar* timestamp_string = xmlGetProp(transaction_node, (const xmlChar*) "timestamp");
    long timestamp;
    if (!timestamp_string)
        timestamp = 0;
    else
        timestamp = atol((char*) timestamp_string);

    xmlFree(type_string);
    xmlFree(timestamp_string);

    Transaction* transaction = transaction_alloc(type, timestamp);

    xmlNode *balance_deltas_node = NULL,
    *packs_deltas_node = NULL,
    *counter_deltas_node = NULL;

    // get nodes
    for (xmlNode *node = transaction_node->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "balance_deltas") == 0) {
            balance_deltas_node = node;
        } else if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "packs_deltas") == 0) {
            packs_deltas_node = node;
        } else if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "counter_deltas") == 0) {
            counter_deltas_node = node;
        }
    }

    if (balance_deltas_node) {
        transaction->balance_delta_head = xml_parse_balance_deltas(balance_deltas_node, persons, settings);
    }
    if (packs_deltas_node) {
        transaction->packs_delta_head = xml_parse_packs_deltas(packs_deltas_node);
    }
    if (counter_deltas_node) {
        transaction->counter_delta_head = xml_parse_counter_deltas(counter_deltas_node, persons);
    }

    return transaction;
}

Transaction* xml_parse_transactions(const xmlNode* transactions_node, Person* persons, const Settings* settings)
{
    Transaction *transactions = NULL;
    for (xmlNode *transaction_node = transactions_node->children; transaction_node; transaction_node = transaction_node->next) {
        if (transactions_node->type == XML_ELEMENT_NODE && xmlStrcmp(transaction_node->name, (const xmlChar*) "transaction") == 0) {
            Transaction* t = xml_parse_transaction(transaction_node, persons, settings);
            if (!t) continue;
            transaction_add(&transactions, t);
        }
    }
    return transactions;
}

Person* xml_parse_person(const xmlNode* person_node, const Settings* settings)
{
    xmlChar *name = xmlGetProp(person_node, (const xmlChar*) "name");
    xmlChar *balance = xmlGetProp(person_node, (const xmlChar*) "balance");
    xmlChar *thirst = xmlGetProp(person_node, (const xmlChar*) "thirst");
    xmlChar *current_coffees = xmlGetProp(person_node, (const xmlChar*) "current_coffees");
    xmlChar *total_coffees = xmlGetProp(person_node, (const xmlChar*) "total_coffees");

    if (!name || !balance || !thirst || !current_coffees || !total_coffees) {
        xmlFree(name);
        xmlFree(balance);
        xmlFree(thirst);
        xmlFree(current_coffees);
        xmlFree(total_coffees);
        return NULL;
    }

    Person *p = person_create_full(
        (char*)name, 
        atoi((char*)balance), 
        settings->currency, 
        atof((char*)thirst), 
        atoi((char*)current_coffees), 
        atoi((char*)total_coffees));

    xmlFree(name);
    xmlFree(balance);
    xmlFree(thirst);
    xmlFree(current_coffees);
    xmlFree(total_coffees);
    return p;
}

Person* xml_parse_persons(const xmlNode* persons_node, const Settings* settings)
{
    Person *persons = NULL;
    for (xmlNode *person_node = persons_node->children; person_node; person_node = person_node->next) {
        if (person_node->type == XML_ELEMENT_NODE && xmlStrcmp(person_node->name, (const xmlChar*) "person") == 0) {
            Person *p = xml_parse_person(person_node, settings);
            if (!p) continue;
            person_add(&persons, p);
        }
    }
    return persons;
}

Kitty* xml_parse_kitty(const xmlNode* node, Person* persons, Settings* settings, Transaction* transactions)
{
    xmlChar *balance = xmlGetProp(node, (const xmlChar*) "balance");
    xmlChar *price = xmlGetProp(node, (const xmlChar*) "price");
    xmlChar *packs = xmlGetProp(node, (const xmlChar*) "packs");
    xmlChar *counter = xmlGetProp(node, (const xmlChar*) "counter");

    if (!balance || !price || !packs || !counter) {
        xmlFree(balance);
        xmlFree(price);
        xmlFree(packs);
        xmlFree(counter);
        return NULL;
    }

    Kitty *kitty = create_kitty(
        atoi((char*)balance),
        atoi((char*)price),
        atoi((char*)packs),
        atoi((char*)counter),
        settings, persons, transactions);

    xmlFree(balance);
    xmlFree(price);
    xmlFree(packs);
    xmlFree(counter);

    return kitty;
}

Currency* xml_parse_currency(const xmlNode* currency_node)
{
    xmlChar *isoname = xmlGetProp(currency_node, (const xmlChar*) "isoname");
    xmlChar *prefix = xmlGetProp(currency_node, (const xmlChar*) "prefix");
    xmlChar *subunit_digits = xmlGetProp(currency_node, (const xmlChar*) "subunit_digits");
    xmlChar *decimal = xmlGetProp(currency_node, (const xmlChar*) "decimal");

    if (!isoname || !subunit_digits || !decimal || !prefix) {
        xmlFree(isoname);
        xmlFree(prefix);
        xmlFree(subunit_digits);
        xmlFree(decimal);
        return NULL;
    }

    Currency *c = currency_alloc(
        (char*)isoname,
        (bool) atoi((char*) prefix),
        atoi((char*)subunit_digits),
        decimal[0]);

    xmlFree(isoname);
    xmlFree(subunit_digits);
    xmlFree(decimal);
    xmlFree(prefix);

    return c;
}

Settings* xml_parse_settings(const xmlNode* settings_node)
{
    xmlNode *currency_node = NULL;

    for (xmlNode *node = settings_node -> children; node; node=node->next) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "currency") == 0) {
            currency_node = node;
        }
    }

    if (!currency_node) {
        return NULL;
    }

    Currency *c = xml_parse_currency(currency_node);
    return settings_alloc(c);
}

const char* get_config_directory()
{
    const char* directory = ".coffeekitty";
    const char* home = getenv("HOME");

    _Thread_local static char rval[PATH_MAX];
    if (!home) {
        snprintf(rval, PATH_MAX, "%s", directory);
    } else {
        snprintf(rval, PATH_MAX, "%s/%s", home, directory);
    }
    return rval;
}

const char* get_config_file_path()
{
    const char* filename = "data.xml";

    _Thread_local static char rval[PATH_MAX];
    snprintf(rval, PATH_MAX, "%s/%s", get_config_directory(), filename);
    return rval;
}

int mkdir_p(const char *path)
{
    char buffer[PATH_MAX + sizeof("mkdir -p ")];
    snprintf(buffer, sizeof(buffer), "mkdir -p %s", path);
    return system(buffer);
}

Kitty *load_kitty_from_xml(const char *path)
{
    xmlDocPtr doc = xmlReadFile(path, NULL, 0);
    if (!doc) {
        fprintf(stderr, "Failed to parse %s\n", path);
        return NULL;
    }

    xmlNode *settings_node = NULL,
    *kitty_node = NULL,
    *persons_node = NULL,
    *transactions_node = NULL;

    // get nodes
    xmlNode *root = xmlDocGetRootElement(doc);
    for (xmlNode *node = root->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "settings") == 0) {
            settings_node = node;
        } else if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "kitty") == 0) {
            kitty_node = node;
        } else if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "persons") == 0) {
            persons_node = node;
        } else if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar*) "transactions") == 0) {
            transactions_node = node;
        }
    }

    if (settings_node == NULL || kitty_node == NULL || persons_node == NULL || transactions_node == NULL) {
        fprintf(stderr, "Failed to parse %s\n", path);
        return NULL;
    }

    Settings *settings = xml_parse_settings(settings_node);
    if (!settings) {
        return NULL;
    }

    Person *persons = xml_parse_persons(persons_node, settings);
    Transaction *transactions = xml_parse_transactions(transactions_node, persons, settings); 

    Kitty *kitty = xml_parse_kitty(kitty_node, persons, settings, transactions);
    if (!kitty) {
        return NULL;
    }

    xmlFreeDoc(doc);

    return kitty;
}

/* saving functions */

xmlNodePtr xml_create_currency_node(xmlNodePtr parent, const Currency *currency)
{
    char buffer[20];
    xmlNodePtr currency_node = xmlNewChild(parent, NULL, (const xmlChar*) "currency", NULL);
    xmlNewProp(currency_node, (const xmlChar*) "isoname", (const xmlChar*) currency->isoname);
    snprintf(buffer, sizeof(buffer), "%i", currency->prefix);
    xmlNewProp(currency_node, (const xmlChar*) "prefix", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%i", currency->subunit_digits);
    xmlNewProp(currency_node, (const xmlChar*) "subunit_digits", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%c", currency->decimal);
    xmlNewProp(currency_node, (const xmlChar*) "decimal", (const xmlChar*) buffer);

    return currency_node;
}

xmlNodePtr xml_create_settings_node(xmlNodePtr parent, const Settings* settings)
{
    xmlNodePtr settings_node = xmlNewChild(parent, NULL, (const xmlChar*) "settings", NULL);
    xml_create_currency_node(settings_node, settings->currency);

    return settings_node;
}

xmlNodePtr xml_create_kitty_node(xmlNodePtr parent, const Kitty* kitty)
{
    xmlNodePtr kitty_node = xmlNewChild(parent, NULL, (const xmlChar*) "kitty", NULL);
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%i", kitty->balance->value);
    xmlNewProp(kitty_node, (const xmlChar*) "balance", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%i", kitty->price->value);
    xmlNewProp(kitty_node, (const xmlChar*) "price", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%i", kitty->packs);
    xmlNewProp(kitty_node, (const xmlChar*) "packs", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%i", kitty->counter);
    xmlNewProp(kitty_node, (const xmlChar*) "counter", (const xmlChar*) buffer);

    return kitty_node;
}

xmlNodePtr xml_create_person_node(xmlNodePtr parent, const Person* person)
{
    xmlNodePtr person_node = xmlNewChild(parent, NULL, (const xmlChar*) "person", NULL);
    char buffer[20];
    xmlNewProp(person_node, (const xmlChar*) "name", (const xmlChar*) person->name);
    snprintf(buffer, sizeof(buffer), "%i", person->balance->value);
    xmlNewProp(person_node, (const xmlChar*) "balance", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%f", person->thirst);
    xmlNewProp(person_node, (const xmlChar*) "thirst", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%i", person->current_coffees);
    xmlNewProp(person_node, (const xmlChar*) "current_coffees", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%i", person->total_coffees);
    xmlNewProp(person_node, (const xmlChar*) "total_coffees", (const xmlChar*) buffer);

    return person_node;
}

xmlNodePtr xml_create_persons_node(xmlNodePtr parent, const Person* persons)
{
    xmlNodePtr persons_node = xmlNewChild(parent, NULL, (const xmlChar*) "persons", NULL);
    for (const Person *p = persons; p; p = p->next) {
        xml_create_person_node(persons_node, p);
    }
    return persons_node;
}

xmlNodePtr xml_create_balance_delta_node(xmlNodePtr parent, const BalanceDelta* delta)
{
    xmlNodePtr delta_node = xmlNewChild(parent, NULL, (const xmlChar*) "balance_delta", NULL);
    char buffer[20];
    if (delta->target) {
        xmlNewProp(delta_node, (const xmlChar*) "target", (const xmlChar*) delta->target->name);
    } // else, target will be not set
    snprintf(buffer, sizeof(buffer), "%i", delta->cv->value);
    xmlNewProp(delta_node, (const xmlChar*) "value", (const xmlChar*) buffer);

    return delta_node;
}

xmlNodePtr xml_create_balance_deltas_node(xmlNodePtr parent, const BalanceDelta* delta)
{
    if (!delta) {
        return NULL;
    }
    xmlNodePtr balance_deltas_node = xmlNewChild(parent, NULL, (const xmlChar*) "balance_deltas", NULL);
    for(const BalanceDelta *b = delta; b; b = b->next) {
        xml_create_balance_delta_node(balance_deltas_node, b);
    }

    return balance_deltas_node;
}

xmlNodePtr xml_create_packs_delta_node(xmlNodePtr parent, const PacksDelta* delta)
{
    xmlNodePtr delta_node = xmlNewChild(parent, NULL, (const xmlChar*) "packs_delta", NULL);
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%i", delta->packs);
    xmlNewProp(delta_node, (const xmlChar*) "value", (const xmlChar*) buffer);

    return delta_node;
}

xmlNodePtr xml_create_packs_deltas_node(xmlNodePtr parent, const PacksDelta* delta)
{
    if (!delta) {
        return NULL;
    }
    xmlNodePtr packs_deltas_node = xmlNewChild(parent, NULL, (const xmlChar*) "packs_deltas", NULL);
    for(const PacksDelta *p = delta; p; p = p->next) {
        xml_create_packs_delta_node(packs_deltas_node, p);
    }

    return packs_deltas_node;
}

xmlNodePtr xml_create_counter_delta_node(xmlNodePtr parent, const CounterDelta* delta)
{
    xmlNodePtr delta_node = xmlNewChild(parent, NULL, (const xmlChar*) "counter_delta", NULL);
    char buffer[20];
    if (delta->target) {
        xmlNewProp(delta_node, (const xmlChar*) "target", (const xmlChar*) delta->target->name);
    } // else, target will be not set
    snprintf(buffer, sizeof(buffer), "%i", delta->counter);
    xmlNewProp(delta_node, (const xmlChar*) "value", (const xmlChar*) buffer);

    return delta_node;
}

xmlNodePtr xml_create_counter_deltas_node(xmlNodePtr parent, const CounterDelta* delta)
{
    if (!delta) {
        return NULL;
    }
    xmlNodePtr counter_deltas_node = xmlNewChild(parent, NULL, (const xmlChar*) "counter_deltas", NULL);
    for(const CounterDelta *c = delta; c; c = c->next) {
        xml_create_counter_delta_node(counter_deltas_node, c);
    }

    return counter_deltas_node;
}

xmlNodePtr xml_create_transaction_node(xmlNodePtr parent, const Transaction* transaction)
{
    xmlNodePtr transaction_node = xmlNewChild(parent, NULL, (const xmlChar*) "transaction", NULL);
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%i", transaction->type);
    xmlNewProp(transaction_node, (const xmlChar*) "type", (const xmlChar*) buffer);
    snprintf(buffer, sizeof(buffer), "%li", transaction->timestamp);
    xmlNewProp(transaction_node, (const xmlChar*) "timestamp", (const xmlChar*) buffer);
    xml_create_balance_deltas_node(transaction_node, transaction->balance_delta_head);
    xml_create_packs_deltas_node(transaction_node, transaction->packs_delta_head);
    xml_create_counter_deltas_node(transaction_node, transaction->counter_delta_head);

    return transaction_node;
}

xmlNodePtr xml_create_transactions_node(xmlNodePtr parent, const Transaction* transactions)
{
    xmlNodePtr transactions_node = xmlNewChild(parent, NULL, (const xmlChar*) "transactions", NULL);
    for (const Transaction *t = transactions; t; t = t->next) {
        xml_create_transaction_node(transactions_node, t);
    }

    return transactions_node;
}

int save_kitty_to_xml(const char* path, const Kitty* kitty)
{
    xmlDocPtr doc = xmlNewDoc((const xmlChar*) "1.0");
    if (!doc) {
        fprintf(stderr, "Failed to create xml document\n");
        return 1;
    }


    xmlNodePtr root = xmlNewNode(NULL, (const xmlChar*) "data");
    xmlDocSetRootElement(doc, root);

    xmlNodePtr storage_info_node = xmlNewChild(root, NULL, (const xmlChar*) "storage_info", NULL);
    xmlNewProp(storage_info_node, (const xmlChar*) "version", (const xmlChar*) KITTY_STORAGE_VERSION);
    xml_create_settings_node(root, kitty->settings);
    xml_create_kitty_node(root, kitty);
    xml_create_persons_node(root, kitty->persons);
    xml_create_transactions_node(root, kitty->transactions);

    xmlSaveFormatFileEnc(path, doc, "UTF-8", 1);
    xmlFreeDoc(doc);

    return 0;
}