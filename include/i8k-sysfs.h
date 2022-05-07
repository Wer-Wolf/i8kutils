/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef I8K_SYSFS_H
#define I8K_SYSFS_H

int sysfs_init_fan(int fan, int *fd);
int sysfs_init_hwmon(int *fd);

int sysfs_set_fan_state(int cooling_device_dirfd, int state);
int sysfs_get_fan_state(int cooling_device_dirfd, int *state);

int sysfs_get_fan_speed(int hwmon_dirfd, int fan, int *speed);

int sysfs_get_temp(int hwmon_dirfd, int channel, int *temp);

#endif
