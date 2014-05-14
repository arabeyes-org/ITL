#!/bin/sh

# autogen.sh from Valgrind

run ()
{
    echo "running: $*"
    eval $*

    if test $? != 0 ; then
	echo "error: while running '$*'"
	exit 1
    fi
}

run autoreconf -if
