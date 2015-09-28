#!/bin/sh
#
# Create test files for B Tree test harness (bt)
#

if [ ! -x ./gendef ]; then
	echo "$0: gendef binary cannot be found"
	exit 1
fi

if [ "$1" == "" ]; then
	GENCNT=1000
else
	GENCNT=$1
fi
./gendef $GENCNT >inskeysrand.bt
sort inskeysrand.bt >inskeysord.bt
sort -r inskeysrand.bt >inskeysrord.bt
sed -e 's/^d/r/' <inskeysrand.bt >delkeysrand.bt
sort delkeysrand.bt >delkeysord.bt
sort -r delkeysrand.bt >delkeysrord.bt
sed -e 's/^d/f/' <inskeysrand.bt >fndkeysrand.bt
sort fndkeysrand.bt >fndkeysord.bt
sort -r fndkeysrand.bt >fndkeysrord.bt
