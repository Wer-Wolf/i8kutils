/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <errno.h>

#ifndef I8K_PROCFS_H
#define I8K_PROCFS_H

#ifdef HAVE_I8K_PROCFS

int procfs_init(int *fd);

int procfs_set_fan_state(int fd, int fan, int state);
int procfs_get_fan_state(int fd, int fan, int *state);

#else

int procfs_init(int *fd) { return ENOTSUP; }

int procfs_set_fan_state(int fd, int fan, int state) { return ENOTSUP; }
int procfs_get_fan_state(int fd, int fan, int *state) { return ENOTSUP; }

#endif /* HAVE_I8K_PROCFS */

#endif	/* I8k_PROCFS_H */
