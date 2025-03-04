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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "currency.h"

#define MAX_SETTINGS_ENTRY_KEY_LENGTH 128

typedef struct Settings{
    Currency *currency;
} Settings;

typedef struct SettingsEntry{
    char key[MAX_SETTINGS_ENTRY_KEY_LENGTH];
    void *value;
    struct SettingsEntry *next;
} SettingsEntry;

Settings* create_settings(Currency* c);
void free_settings(Settings* s);

#endif