# Package up for adoption

https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=948521

https://www.debian.org/devel/wnpp/rfa






I8KUTILS
========

OVERVIEW
========

i8kutils is a collection of utilities for controlling the fans on some Dell
laptops. The utilities are entirely built upon the `dell-smm-hwmon` kernel
module.

The i8kutils package includes the following utilities:

* i8kctl, a command-line utility for interfacing with the kernel module.
* i8kmon, a temperature monitor with fan control capability.
* i8kfan, a utility to set the state (speed) of fans.

Since 2011 (kernel version 3.0), the kernel module exports temperature and
fan data over the standard linux hwmon interface. If you are running a recent
enough kernel, you might want to take a look at the [lm-sensors project](https://github.com/lm-sensors/lm-sensors).

The i8kctl perform queries and sets related to fan control as
read temperature, turn the fan on. The i8kmon continuously monitor the
system temperature and control automatically the fans.

All Dell laptop has the feature of controlling the temperature in the BIOS, but
to some models this feature does not work properly. i8kmon does essentially the same
job as the BIOS is supposed to do.

The latest version of the i8kutils package can be retrieved at:

    https://launchpad.net/i8kutils

The module is supposed to be loaded in the system to i8kmon service starts.


LICENCE
=======

This software is released under the terms of the GNU General Public
Licence.

   Copyright (C) 2001-2009 Massimo Dal Zotto <dz@debian.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.

On Debian GNU/Linux systems, the complete text of the GNU General
Public License can be found in `/usr/share/common-licenses/GPL'.


THE KERNEL MODULE
=================

The information provided by the kernel module can be accessed by simply
reading the /proc/i8k file. For example:

    $ cat /proc/i8k
    1.0 A17 2J59L02 52 2 1 8040 6420 1 2

The fields read from /proc/i8k are:

    1.0 A17 2J59L02 52 2 1 8040 6420 1 2
    |   |   |       |  | | |    |    | |
    |   |   |       |  | | |    |    | +------- 10. buttons status
    |   |   |       |  | | |    |    +--------- 9.  ac status
    |   |   |       |  | | |    +-------------- 8.  right fan rpm
    |   |   |       |  | | +------------------- 7.  left fan rpm
    |   |   |       |  | +--------------------- 6.  right fan status
    |   |   |       |  +----------------------- 5.  left fan status
    |   |   |       +-------------------------- 4.  CPU temperature (Celsius)
    |   |   +---------------------------------- 3.  Dell service tag (later known as 'serial number')
    |   +-------------------------------------- 2.  BIOS version
    +------------------------------------------ 1.  /proc/i8k format version

A negative value, for example -22, indicates that the BIOS doesn't return
the corresponding information. This is normal on some models/bioses.

For performance reasons the /proc/i8k doesn't report by default the ac status
since this SMM call takes a long time to execute and is not really needed.
If you want to see the ac status in /proc/i8k you must explictitly enable
this option by passing the "power_status=1" parameter to insmod. If ac status
is not available -1 is printed instead.

The driver provides also an ioctl interface which can be used to obtain the
same information and to control the fan status. The ioctl interface can be
accessed from C programs or from shell using the i8kctl utility. See the
source file i8kctl.c for more information on how to use the ioctl interface.

If the /proc/i8k file does not exist, then check wether the kernel module
is loaded and the kernel has the config option `CONFIG_I8K` enabled.

The documentation of the `dell_smm_hwmon` kernel driver can be found
[here](https://www.kernel.org/doc/html/latest/hwmon/dell-smm-hwmon.html).

The driver accepts the following parameters:

* ignore_dmi=1
    * forces the driver to load on unknown hardware

* force=1
    * forces the driver to load on unsupported/buggy hardware
    * might cause problems since it also disables all blacklists
      for buggy hardware, use only when ignore_dmi=1 is not enough

* restricted=1
    * allows privileged programs to change fan speed
    * **do not use, since it allows malicous programs to damage your hardware
      by disabling fan control**

* power_status=1
    * report ac status in /proc/i8k

* fan_mult=<int>
    * overrides the fan speed multiplicator

* fan_max=<int>
    * overrides the maximum fan state.

You can specify the module parameters when loading the module or as kernel
option when booting the kernel if the driver is compiled statically.

To have the module loaded automatically at boot you must manually add the
line "dell-smm-hwmon" into the file /etc/modules or use the modconf utility. For example:

    $ cat /etc/modules
    # /etc/modules: kernel modules to load at boot time.
    #
    # This file contains the names of kernel modules that should be loaded
    # at boot time, one per line. Lines beginning with "#" are ignored.
    dell-smm-hwmon

Any module parameters must be specified in /etc/modprobe.d/dell-smm-hwmon.conf.
To force dell-smm-hwmon to load on unknown hardware, the above file should
contain the following line:

    options dell-smm-hwmon force=1


THE I8KCTL UTILITY
==================

The i8kctl utility provides a command-line interface to the `dell-smm-hwmon` kernel driver.
When invoked without arguments the program reports the same information which
can be read from the /proc/i8k file.

In order to modify fan speeds, the utility requires root privileges.

The program can take an optional argument which can be used to select only one
of the items and to control the fan status.

The `sensors` utility from lm-sensors provides similar data.


PREREQUISITE
============

i8kutils depends on Tcl and the logger module in tcllib, which may not
installed by default on your system.
You can install them depending on your distro.

Ubuntu

    sudo apt install tcl
    sudo apt install tcllib

Arch Linux

    sudo pacman -S tcl
    sudo pacman -S tcllib

Gentoo

    sudo emerge --ask dev-lang/tcl
    sudo emerge --ask dev-tcltk/tcllib


COMPILATION
===========

To compile the programs type the following commands:

    meson build --prefix="/usr"
    cd build
    meson compile


INSTALLATION
============

To install i8kutils, type the following commands:

    sudo meson install

You must then manually install the provided init scripts if necessary.
For enabling i8kmon to read the battery status, you must also install
'acpitool' or 'acpi', otherwise i8kmon will assume that it always
runs on ac power.

CONTRIBUTORS
============

Contributors are listed here, in alphabetical order.

* Pablo Bianucci <pbian@physics.utexas.edu>
    * support for /proc/acpi

* David Bustos <bustos@caltech.edu>
    * patches for generating keyboard events

* Jonathan Buzzard <jonathan@buzzard.org.uk>
    * basic information on the SMM BIOS and the Toshiba SMM driver
    * Asm code for calling the SMM BIOS on the I8K. Without his help
      this work wouldn't have been possible.

* Karl E. Jørgensen <karl@jorgensen.com>
    * init script for i8kmon daemon

* Stephane Jourdois <stephane@tuxfinder.org>
    * patches for correctly interpreting buttons status in the i8k driver

* Marcel J.E. Mol <marcel@mesa.nl>
    * patches for the --repeat option in the i8kbuttons (obsolete on Abr 30, 2014) util

* Gianni Tedesco <gianni@ecsc.co.uk>
    * patch to restrict fan contol to SYS_ADMIN capability

* David Woodhouse <dwmw2@redhat.com>
    * suggestions on how to avoid the zombies in i8kbuttons (obsolete on Abr 30, 2014)

* Vitor Augusto <vitorafsr@gmail.com>
    * fixes for the freeze bug at i8kmon, general update and bug fixes

and many others who tested the driver on their hardware and sent reports
and patches.

No credits to DELL Computer who has always refused to give support on Linux
or provide any useful information on the I8K buttons and their buggy BIOS.

---
Massimo Dal Zotto <dz@debian.org>
