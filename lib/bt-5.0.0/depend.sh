#!/bin/sh
SRCDIR=$1
shift
gcc -MM -MG "$@" |sed -e "s!^\(.*\.o\)!${SRCDIR}/\1!"