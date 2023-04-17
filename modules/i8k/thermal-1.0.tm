# thermal -- Tcl module for accessing the thermal sysfs interface.
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

namespace eval thermal {
    oo::class create fan {
        variable channel
        variable index
        variable max

        constructor {stateChannel fanIndex maxState} {
            if {[string is wideinteger -strict $fanIndex] == 0} {
                throw {FAN {invalid index}} "invalid cooling device index \"$fanIndex\""
            }

            if {[string is wideinteger -strict $maxState] == 0} {
                throw {FAN {invalid max. state}} "invalid cooling device maximum state \"$maxState\""
            }

            chan configure $stateChannel -buffering none -translation binary
            set channel $stateChannel
            set index $fanIndex
            set max $maxState

            return
        }

        method getState {} {
            chan seek $channel 0

            return [chan read -nonewline $channel]
        }

        method setState {state} {
            if {$state > $max} {
                throw {FAN {invalid state}} "cooling state $state exceeds max. cooling state $max"
            }

            chan puts -nonewline $channel $state

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

    proc SetupFan {path} {
        set statePath "/cur_state"
        set maxPath "/max_state"
        set typePath "/type"

        set type [string trimright [fileutil::cat -translation binary $path$typePath]]

        if {[string equal -length 12 $type "dell-smm-fan"] == 0} {
            throw {FAN {invalid type}} "invalid cooling device type \"$type\""
        }

        set index [string range $type 12 end]
        set max [string trimright [fileutil::cat -translation binary $path$maxPath]]
        try {
            set channel [open $path$statePath "rb+"]
        } trap {POSIX EACCES} {} {
            set channel [open $path$statePath "rb"]
        }

        return [fan new $channel $index $max]
    }

    generator define detectFans {{thermalDirectory "/sys/class/thermal"}} {
        foreach path [glob -directory $thermalDirectory -types {d r} -nocomplain "cooling_device*"] {
            try {
                generator::yield [SetupFan $path]
            } trap {FAN} {} {
                continue
            }
        }

        return
    }
}
