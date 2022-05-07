// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * i8k-sysfsfs.c -- Routines for retriving sensor data over the sysfs interface
 *
 * Copyright (C) 2022 Armin Wolf <W_Armin@gmx.de>
 */

#define _GNU_SOURCE
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include "i8k-generic.h"
#include "i8k-sysfs.h"

#define THERMAL_CLASS_PATH	"/sys/class/thermal/"
#define HWMON_CLASS_PATH	"/sys/class/hwmon/"

#define SYSFS_NAME_LENGTH	64

static int read_string_at(int dirfd, const char *pathname, char *str, size_t length)
{
	ssize_t bytes;
	int err, fd;

	fd = openat(dirfd, pathname, O_RDONLY);
	if (fd < 0)
		return errno;

	bytes = read(fd, str, length);
	err = errno;
	close(fd);
	if (bytes < 0)
		return err;

	/* output can be newline-terminated */
	if (bytes > 0 && str[bytes - 1] == '\n')
		str[bytes - 1] = '\0';
	else
		str[bytes] = '\0';

	return 0;
}

static int write_string_at(int dirfd, const char *pathname, const char *str)
{
	size_t length = strlen(str);
	ssize_t bytes;
	int err, fd;

	fd = openat(dirfd, pathname, O_WRONLY);
	if (fd < 0)
		return fd;

	bytes = write(fd, str, length);
	err = errno;
	close(fd);
	if (bytes < 0)
		return err;

	if (bytes != length)
		return EIO;

	return 0;
}

static int search_dir(DIR *dir, char *subfile, char *content, int *fd)
{
	char buf[SYSFS_NAME_LENGTH + 1];
	int dir_fd = dirfd(dir);
	struct dirent *entry;
	int subfd, ret;

	if (dir_fd < 0)
		return errno;

	while(1) {
		entry = readdir(dir);
		if (!entry)
			return ENOENT;

		subfd = openat(dir_fd, entry->d_name, O_DIRECTORY);
		if (subfd < 0)
			continue;

		ret = read_string_at(subfd, subfile, buf, sizeof(buf) - 1);
		if (!ret && !strncmp(content, buf, sizeof(buf) - 1))
			break;

		close(subfd);
	}

	*fd = subfd;

	return 0;
}

int sysfs_init_hwmon(int *fd)
{
	DIR *hwmon_dir = opendir(HWMON_CLASS_PATH);
	int ret;

	if (!hwmon_dir)
		return errno;

	ret = search_dir(hwmon_dir, "name", "dell_smm", fd);
	closedir(hwmon_dir);

	return ret;
}

int sysfs_init_fan(int fan, int *fd)
{
	DIR *thermal_dir = opendir(THERMAL_CLASS_PATH);
	char *type;
	int ret;

	ret = asprintf(&type, "dell-smm-fan%d", fan + 1);
	if (ret < 0)
		return errno;

	ret = search_dir(thermal_dir, "type", type, fd);
	closedir(thermal_dir);
	free(type);

	return ret;
}

int sysfs_set_fan_state(int cooling_device_dirfd, int state)
{
	char buf[MAX_DIGITS_PER_INT + 1];
	int ret;

	ret = itostr(state, buf, sizeof(buf) - 1);
	if (ret)
		return ret;

	return write_string_at(cooling_device_dirfd, "cur_state", buf);
}

int sysfs_get_fan_state(int cooling_device_dirfd, int *state)
{
	char buf[MAX_DIGITS_PER_LONG + 1];
	int ret;

	ret = read_string_at(cooling_device_dirfd, "cur_state", buf, sizeof(buf) + 1);
	if (ret)
		return ret;

	return strtoi(buf, state);
}

int sysfs_get_fan_speed(int hwmon_dirfd, int fan, int *speed)
{
	char buf[MAX_DIGITS_PER_LONG + 1];
	char *fan_name;
	int ret;

	ret = asprintf(&fan_name, "fan%d_input", fan + 1);
	if (ret < 0)
		return ret;

	ret = read_string_at(hwmon_dirfd, fan_name, buf, sizeof(buf) - 1);
	free(fan_name);
	if (ret)
		return ret;

	return strtoi(buf, speed);
}

int sysfs_get_temp(int hwmon_dirfd, int channel, int *temp)
{
	char buf[MAX_DIGITS_PER_LONG + 1];
	char *temp_name;
	int value, ret;

	ret = asprintf(&temp_name, "temp%d_input", channel + 1);
	if (ret < 0)
		return errno;

	ret = read_string_at(hwmon_dirfd, temp_name, buf, sizeof(buf) - 1);
	free(temp_name);
	if (ret)
		return ret;

	ret = strtoi(buf, &value);
	if (ret)
		return ret;

	*temp = value / 1000;

	return 0;
}
