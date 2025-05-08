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

#include "latex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kitty.h"
#include "currency.h"

const char* LATEX_PREAMBLE =
"\\documentclass[landscape]{article}\n"
"\\usepackage[a4paper, margin=1in]{geometry}\n"
"\\usepackage{pdflscape}\n"
"\\usepackage{tabularx}\n"
"\\usepackage{color}\n"
"\\usepackage{fancyhdr}\n"
"\n"
"\\begin{document}\n"
"\\pagestyle{empty}\n";

const char* LATEX_TABLE_HEADER =
"\t\\begin{table}[ht]\n"
"\t\t\\centering\n"
"\t\t\\renewcommand{\\arraystretch}{1.5}\n"
"\t\t\\begin{tabularx}{\\textwidth}{|l|r|X|p{3cm}|}\n"
"\t\t\t\\hline\n";

const char* LATEX_TABLE_FOOTER =
"\t\t\t\\hline\n"
"\t\t\\end{tabularx}\n"
"\t\\end{table}\n";

const char* LATEX_EPILOG =
"\\end{document}\n";

const char* T1 = "\t";
const char* T2 = "\t\t";
const char* T3 = "\t\t\t";

const int MIN_ROW_HEIGHT_IN_EX = 3;
const int MAX_TOTAL_ROW_HEIGHT_IN_EX = 64;

void fprint_latex_title(FILE* file, const char* title)
{
    fprintf(file, "%s\\begin{center}\n", T1);
    fprintf(file, "%s\\textbf{%s}\n", T2, title);
    fprintf(file, "%s\\end{center}\n", T1);
}

void fprint_latex_kitty_properties(FILE* file, Kitty *kitty)
{
    fprintf(file, "%s\\begin{tabular}{l  l  l}\n", T1);
    fprintf(file, "%sTotal Balance: & %s & + %i Packs\\\\\n", T2, currency_value_format(kitty->balance, false, true), kitty->packs);
    fprintf(file, "%sCounter: & %d & \\\\\n", T2, kitty->counter);
    fprintf(file, "%sPrice: & %s/Coffee & \\\\\n", T2, currency_value_format(kitty->price, false, true));
    fprintf(file, "%sLast Settlement: & & \\\\\n", T2);
    fprintf(file, "%s\\end{tabular}\n", T1);
}

int fprint_new_latex_sheet(FILE* file, Kitty *kitty)
{
    fprintf(file, "%s", LATEX_PREAMBLE);

    fprint_latex_title(file, "Coffee Fund");

    fprint_latex_kitty_properties(file, kitty);

    // TABLE
    fprintf(file, "%s", LATEX_TABLE_HEADER);
    fprintf(file, "%s\\textbf{Name} & \\textbf{Balance}/%s & & \\textbf{Expenses} \\\\\n", T3, kitty->settings->currency->isoname);
    fprintf(file, "%s\\hline\n", T3);

    float distributable_row_height = MAX_TOTAL_ROW_HEIGHT_IN_EX - MIN_ROW_HEIGHT_IN_EX * get_person_count(kitty->persons);

    for (Person* p = kitty->persons; p != NULL; p = p->next) {
        float row_height = MIN_ROW_HEIGHT_IN_EX + (int) (distributable_row_height * p->thirst);
        fprintf(file, "%s\\hline\n", T3);

        char balance_color_modifier[128] = "{";
        char balance_boldness_modifier[128] = "{";
        if (p->balance->value < 0) {
            snprintf(balance_color_modifier, sizeof(balance_color_modifier), "\\textcolor{red}{");
            if (p->balance->value < -10000) {
                snprintf(balance_boldness_modifier, sizeof(balance_boldness_modifier), "\\textbf{");
            }
        }

        fprintf(file, "%s%s & %s%s%s%s%s & \\rule{0pt}{%fex} &  \\\\\n",
            T3, p->name,
            balance_color_modifier, balance_boldness_modifier,
            currency_value_format(p->balance, false, false),
            "}","}",
            row_height);
    }
    fprintf(file, "%s", LATEX_TABLE_FOOTER);

    fprintf(file, "%s", LATEX_EPILOG);
    return 0;
}