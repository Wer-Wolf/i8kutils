// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * i8k-procfs.c -- Routines for retriving sensor data over the procfs interface
 *
 * Copyright (C) 2022 Armin Wolf <W_Armin@gmx.de>
 * Based on work by Massimo Dal Zotto and Vitor Augusto.
 */

#include <errno.h>
#include <stddef.h>

#include <linux/i8k.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#include "i8k-procfs.h"

int procfs_init(int *fd)
{
	int ret = open(I8K_PROC, O_RDONLY | O_NONBLOCK);

	if (ret < 0)
		return errno;

	*fd = ret;

	return 0;
}

int procfs_set_fan_state(int fd, int fan, int state)
{
	int args[2] = {fan, state};

	if (ioctl(fd, I8K_SET_FAN, &args) < 0)
		return errno;

	return 0;
}

int procfs_get_fan_state(int fd, int fan, int *state)
{
	int args[1] = {fan};

	if (ioctl(fd, I8K_GET_FAN, &args) < 0)
		return errno;

	*state = args[0];

	return 0;
}
