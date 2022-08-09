// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * i8k-generic.c -- Routines used in various places inside i8kutils
 *
 * Copyright (C) 2022 Armin Wolf <W_Armin@gmx.de>
 * Based on work by Massimo Dal Zotto and Vitor Augusto.
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "i8k-generic.h"

bool verbose = false;

int strtoi(const char *str, int *value)
{
	long result;
	char *error;

	result = strtol(str, &error, 10);
	if (!(str[0] != '\0' && *error == '\0'))
		return EINVAL;

	if (result < INT_MIN || result > INT_MAX)
		return ERANGE;

	*value = (int)result;

	return 0;
}

int itostr(const int value, char *str, size_t length)
{
	int ret = snprintf(str, length, "%d", value);

	if (ret < 0 || ret >= length)
		return EINVAL;

	return 0;
}

void print_error(int err, const char *str)
{
	errno = err;
	perror(str);
}

int timestamp(double *time)
{
	struct timespec stamp;
	double seconds;
	int ret;

	ret = clock_gettime(CLOCK_MONOTONIC, &stamp);
	if (ret < 0)
		return errno;

	seconds = stamp.tv_nsec;
	seconds /= 1000000000;
	seconds += stamp.tv_sec;

	*time = seconds;

	return 0;
}

void print_duration(const char *operation, double start)
{
	double now;

	if (!verbose)
		return;

	if (timestamp(&now))
		return;

	fprintf(stderr, "%s took %lf seconds\n", operation, now - start);
}
