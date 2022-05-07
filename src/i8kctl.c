// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * i8kctl.c -- Utility to query the i8k kernel module on Dell laptops to
 * retrieve sensor information
 *
 * Copyright (C) 2022 Armin Wolf <W_Armin@gmx.de>
 * Copyright (C) 2013-2017 Vitor Augusto <vitorafsr@gmail.com>
 * Copyright (C) 2001  Massimo Dal Zotto <dz@debian.org>
 */

#include <argp.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "i8k-generic.h"
#include "i8k-procfs.h"
#include "i8k-sysfs.h"

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof((array)[0]))

#define DELL_SMM_NUM_FANS	3
#define DELL_SMM_NUM_TEMP	10

enum sensor_types {
	SENSOR_NONE,
	SENSOR_FAN,
	SENSOR_TACHO,
	SENSOR_TEMP
};

struct sensor_name {
	const char *name;
	const enum sensor_types type;
};

struct arguments {
	enum sensor_types sensor_type;
	int sensor_number;
	int sensor_value;
};

const struct sensor_name sensor_names[] = {
	{
		.name = "fan",
		.type = SENSOR_FAN,
	},
	{
		.name = "tacho",
		.type = SENSOR_TACHO,
	},
	{
		.name = "temp",
		.type = SENSOR_TEMP,
	}
};

const char *argp_program_version = "i8kctl version " VERSION;

const char argp_program_bug_adress[] = "TODO";

const char doc[] = "i8kctl -- a utility for fan control on Dell notebooks";

const char args_doc[] = "[STATE]";

const struct argp_option options[] = {
	{
		.name = "sensor-type",
		.key = 's',
		.arg = "type",
		.doc = "Type of sensor to read/write, can be \"fan\", \"tacho\" or \"temp\"",
	},
	{
		.name = "sensor-number",
		.key = 'n',
		.arg = "number",
		.doc = "Number of sensor to read/write",
	},
	{
		.name = "verbose",
		.key = 'v',
		.doc = "Enable verbose output",
	},
	{ 0 }
};

static error_t parser(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	int number, ret;

	switch (key) {
		case 's':
			for (int i = 0; i < ARRAY_SIZE(sensor_names); i++) {
				if (!strcmp(arg, sensor_names[i].name)) {
					arguments->sensor_type = sensor_names[i].type;
					return 0;
				}
			}

			argp_failure(state, EX_DATAERR, 0, "Invalid sensor type '%s'", arg);

			break;
		case 'n':
			ret = strtoi(arg, &number);
			if (ret)
				argp_failure(state, EX_DATAERR, ret, "Invalid sensor number '%s'", arg);

			if (number < 1)
				argp_failure(state, EX_DATAERR, 0, "Sensor number cannot be less than 1");

			arguments->sensor_number = number - 1;

			break;
		case 'v':
			verbose = true;
			break;
		case ARGP_KEY_INIT:
			arguments->sensor_type = SENSOR_NONE;
			arguments->sensor_number = -1;
			arguments->sensor_value = -1;
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num >= 1)
				argp_error(state, "Too many arguments");

			ret = strtoi(arg, &arguments->sensor_value);
			if (ret)
				argp_failure(state, EX_DATAERR, ret, "Invalid sensor value '%s'", arg);

			break;
		case ARGP_KEY_END:
			if (arguments->sensor_type != SENSOR_NONE) {
				if (arguments->sensor_number < 0)
					argp_failure(state, EX_DATAERR, 0, "Missing sensor number");
			} else {
				if (arguments->sensor_number >= 0 || arguments->sensor_value >= 0)
					argp_failure(state, EX_DATAERR, 0, "Missing sensor type");
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

const struct argp argument_parser = {
	.options = options,
	.parser = parser,
	.args_doc = args_doc,
	.doc = doc,
};

int read_sensor(enum sensor_types type, int number, int *value)
{
	int fd, ret;

	if (type == SENSOR_FAN) {
		ret = procfs_init(&fd);
		if (ret) {
			ret = sysfs_init_fan(number, &fd);
			if (ret)
				return ret;

			ret = sysfs_get_fan_state(fd, value);
		} else {
			ret = procfs_get_fan_state(fd, number, value);
		}
	} else {
		ret = sysfs_init_hwmon(&fd);
		if (ret)
			return ret;

		switch (type) {
		case SENSOR_TACHO:
			ret = sysfs_get_fan_speed(fd, number, value);
			break;
		case SENSOR_TEMP:
			ret = sysfs_get_temp(fd, number, value);
			break;
		default:
			ret = EINVAL;
		}
	}

	close(fd);

	return ret;
}

int write_sensor(enum sensor_types type, int number, int value)
{
	int fd, ret;

	if (type != SENSOR_FAN)
		return EINVAL;

	ret = procfs_init(&fd);
	if (ret) {
		ret = sysfs_init_fan(number, &fd);
		if (ret)
			return ret;

		ret = sysfs_set_fan_state(fd, value);
	} else {
		ret = procfs_set_fan_state(fd, number, value);
	}

	close(fd);

	return ret;
}

void print_overview()
{
	int i, state, speed, temp, ret;

	for (i = 0; i < DELL_SMM_NUM_FANS; i++) {
		ret = read_sensor(SENSOR_FAN, i, &state);
		if (!ret)
			printf("Fan %d state: %d\n", i + 1, state);

		ret = read_sensor(SENSOR_TACHO, i, &speed);
		if (!ret)
			printf("Fan %d speed: %d RPM\n", i + 1, speed);
	}

	for (i = 0; i < DELL_SMM_NUM_TEMP; i++) {
		ret = read_sensor(SENSOR_TEMP, i, &temp);
		if (!ret)
			printf("Temperature %d: %d °C\n", i + 1, temp);
	}
}

int main(int argc, char **argv)
{
	struct arguments arguments = {};
	int value, ret;

	if (argp_parse(&argument_parser, argc, argv, 0, NULL, &arguments))
		exit(EXIT_FAILURE);

	if (arguments.sensor_type == SENSOR_NONE) {
		print_overview();
		exit(EXIT_SUCCESS);
	}

	if (arguments.sensor_value < 0) {
		ret = read_sensor(arguments.sensor_type, arguments.sensor_number, &value);
		if (ret) {
			print_error(ret, "Unable to read sensor");
			exit(EXIT_FAILURE);
		}

		printf("%d\n", value);
	} else {
		ret = write_sensor(arguments.sensor_type, arguments.sensor_number, arguments.sensor_value);
		if (ret) {
			print_error(ret, "Unable to write sensor");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
