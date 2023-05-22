#!/usr/bin/env tclsh
#
# all.tcl -- Wrapper for executing all tcltests and provide the exitcode test protocol.
#
# Copyright (C) 2023        Armin Wolf <W_Armin@gmx.de>
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
package require tcltest

# tcltest does not provide a way to access the test results, so we
# have to filter the test output ourself
oo::class create stdoutMatcher {
    variable match
    variable matched

    constructor {outputMatch} {
        set match $outputMatch
        set matched 0
    }

    method initialize {handle mode} {
        return "initialize finalize read write"
    }

    method finalize {handle} {
        my destroy
    }

    method read {handle buffer} {
        return $buffer
    }

    method write {handle buffer} {
        if {[string first $match $buffer] > -1} {
            set matched 1
        }

        return $buffer
    }

    method hasMatched {} {
        return $matched
    }
}

# We need to make sure that every tests has exclusive access
# to the tmpdir in order to simulate sysfs
tcltest::configure -singleproc true
tcltest::configure -testdir [file dirname [file normalize [info script]]]

# Allow for test configuration over the command line
tcltest::configure {*}$argv

set matcher [stdoutMatcher new "FAILED"]

chan push stdout $matcher
try {
    tcltest::runAllTests

    set result [$matcher hasMatched]
} finally {
    chan pop stdout
}

exit $result
