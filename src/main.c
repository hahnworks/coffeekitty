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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "currency.h"
#include "kitty.h"
#include "storage.h"
#include "commands.h"

void clean_exit(int rval, Kitty* kitty, bool save)
{
    if (save) {
        sort_persons_by_name(&kitty->persons);
        if (save_kitty_to_xml(get_config_file_path(), kitty)) {
            fprintf(stderr, "Failed to save database\n");
            rval = 1;
        }
    }

    if (kitty) {
        if (kitty->persons) {
            free_persons(kitty->persons);
        }
        if (kitty->settings) {
            if (kitty->settings->currency) {
                free_currency(kitty->settings->currency);
            }
            free_settings(kitty->settings);
        }
        free_kitty(kitty);
    }

    exit(rval);
}

int main(int argc, char **argv)
{
    const char* filepath = get_config_file_path();

    if (access(filepath, F_OK)) {
        fprintf(stderr, "Creating database...\n");

        Settings *settings = create_settings(create_currency("EUR", false, 2, '.'));
        Person *persons = NULL;
        Kitty *kitty = create_kitty(0, 25, 0, 0, settings, persons);
        if (mkdir_p(get_config_directory())) {
            fprintf(stderr, "Failed to create directory\n");
            clean_exit(1, kitty, false);
        }
        if (save_kitty_to_xml(filepath, kitty)) {
            fprintf(stderr, "Failed to save database\n");
            clean_exit(1, kitty, false);
        }
    }

    Kitty *kitty = load_kitty_from_xml(get_config_file_path());

    if (!kitty) {
        return 1;
    }

    int rval;
    rval = parse_command(argc, argv, kitty);

    clean_exit(rval, kitty, true);
}
