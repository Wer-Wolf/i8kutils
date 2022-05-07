/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdbool.h>
#include <stddef.h>

#ifndef I8K_GENERIC_H
#define I8K_GENERIC_H

#define MAX_DIGITS_PER_INT      11
#define MAX_DIGITS_PER_LONG     21

extern bool verbose;

int strtoi(const char *str, int *value);

int itostr(const int state, char *str, size_t length);

void print_error(int err, const char *str);

int timestamp(double *time);

void print_duration(const char *operation, double start);

#endif /* I8K_GENERIC_H */
