# Package up for adoption

https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=948521

https://www.debian.org/devel/wnpp/rfa






I8KUTILS
========

OVERVIEW
========

i8kutils is a collection of utilities for controlling the fans on some Dell
laptops. The utilities are entirely built upon the `dell-smm-hwmon` kernel
module, which has to be loaded before they can work.

The i8kutils package includes the following utilities:

* i8kctl, a command-line utility for interfacing with the kernel module.
* i8kmon, a temperature monitor with fan control capability.

Since 2011 (kernel version 3.0), the kernel module exports temperature and
fan data over the standard linux hwmon interface. If you are running a recent
enough kernel, you might want to take a look at the [lm-sensors project](https://github.com/lm-sensors/lm-sensors).
In order to access fan sensors using i8kctl, kernel version 5.19 or greater
is required.

All Dell laptops handle fan control through the BIOS, but on some models this
feature does not work properly. i8kmon does essentially the same job as the
BIOS is supposed to do.

LICENCE
=======

This software is released under the terms of the GNU General Public
Licence, see `COPYING` for details.

THE KERNEL MODULE
=================

The documentation of the `dell_smm_hwmon` kernel driver can be found
[here](https://www.kernel.org/doc/html/latest/hwmon/dell-smm-hwmon.html).

The driver accepts the following parameters:

* ignore_dmi=1
    * forces the driver to load on unknown hardware

* force=1
    * forces the driver to load on unsupported/buggy hardware
    * might cause problems since it also disables all blacklists
      for buggy hardware, use only when ignore_dmi=1 is not enough

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

    options dell-smm-hwmon ignore_dmi=1


THE I8KCTL UTILITY
==================

The i8kctl utility provides a command-line interface to the `dell-smm-hwmon` kernel driver.
When invoked without arguments the program reports the readings of all fan and temperature
sensors exposed by the driver.

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


BUILDING
========

To build the programs enter the following commands:

    meson setup build --prefix="/usr"
    cd build


INSTALLATION
============

To install i8kutils, enter the following command:

    sudo meson install

You must then manually install the provided init scripts if necessary.
For enabling i8kmon to read the battery status, you must also install
'acpi', otherwise i8kmon will assume that it always runs on ac power.

TESTING
=======

To execute some basic tests, enter the following command:

    meson test

This will execute some basic tests and print the results. Please note
that those tests only test the tcl modules used for hardware access,
not the applications itself.

CONTRIBUTORS
============

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

* Massimo Dal Zotto <dz@debian.org>
    * initial developer

and many others who tested the driver on their hardware and sent reports
and patches.

No credits to DELL Computer who has always refused to give support on Linux
or provide any useful information on the I8K buttons and their buggy BIOS.

---
Massimo Dal Zotto <dz@debian.org>
