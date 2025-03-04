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

#ifndef STORAGE_H
#define STORAGE_H

#include "kitty.h"

#define KITTY_STORAGE_VERSION "1.0"

Kitty *load_kitty_from_xml(const char *path);
const char* get_config_directory();
const char* get_config_file_path(); 
int mkdir_p(const char *path);
int save_kitty_to_xml(const char *path, const Kitty *kitty);

#endif