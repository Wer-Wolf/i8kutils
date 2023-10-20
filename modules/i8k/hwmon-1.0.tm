# hwmon -- Tcl module for accessing the hwmon sysfs interface.
#
# Copyright (C) 2023    Armin Wolf <W_Armin@gmx.de>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

package require Tcl 8.6
package require fileutil
package require generator

namespace eval hwmon {
    oo::class create sensor {
        variable channel
        variable index

        constructor {inputChannel sensorIndex} {
            if {[string is wideinteger -strict $sensorIndex] == 0} {
                throw {SENSOR {invalid index}} "invalid sensor index $sensorIndex"
            }

            chan configure $inputChannel -buffering none -translation binary
            set channel $inputChannel
            set index $sensorIndex

            return
        }

        method getTemperature {} {
            chan seek $channel 0

            return [chan read -nonewline $channel]
        }

        method getIndex {} {
            return $index
        }

        destructor {
            chan close $channel

            return
        }
    }

    oo::class create fanMode {
        variable channel
        variable index

        constructor {inputChannel sensorIndex} {
            chan configure $inputChannel -buffering none -translation binary
            set channel $inputChannel
            set index $sensorIndex

            return
        }

        method setMode {mode} {
            switch $mode {
                "manual" {
                    set token 1
                }
                "automatic" {
                    set token 2
                }
                default {
                    throw {FANMODE {invalid mode}} "invalid fan mode $mode"
                }
            }

            chan puts -nonewline $channel $token

            return
        }

        method getIndex {} {
            return $index
        }

        destructor {
            chan close $channel

            return
        }
    }

    proc SetupSensor {path attr} {
        set fullPath [format "%s/%s" $path $attr]
        set index [scan $attr "temp%u_input"]
        set channel [open $fullPath "rb"]

        return [sensor new $channel $index]
    }

    proc getSensor {chipDirectory number} {
        return [SetupSensor $chipDirectory [format "temp%u_input" $number]]
    }

    generator define detectSensors {chipDirectory} {
        foreach attr [glob -directory $chipDirectory -types {f r} -tails -nocomplain "temp*_input"] {
            generator::yield [SetupSensor $chipDirectory $attr]
        }

        return
    }

    proc getFanMode {chipDirectory number} {
        set fullPath [format "%s/pwm%u_enable" $chipDirectory $number]
        set channel [open $fullPath WRONLY]

        return [fanMode new $channel $number]
    }

    proc detectChip {{hwmonDirectory "/sys/class/hwmon"}} {
        foreach chip [glob -directory $hwmonDirectory -types {d r} -nocomplain "hwmon*"] {
            set namePath [format "%s/name" $chip]
            set name [fileutil::cat -translation binary $namePath]

            if {[string equal $name "dell_smm\n"] == 1} {
                return $chip
            }
        }

        throw {HWMON {no chip}} "no suitable hwmon chip found"
    }
}
