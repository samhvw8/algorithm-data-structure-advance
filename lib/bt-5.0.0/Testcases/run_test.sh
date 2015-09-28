#!/bin/sh
DBNAME=test_db
if [ ! -d $1 ]; then
	echo $0: missing testcase directory
	exit 1
fi
# check for message to issue prior to running test
if [ -r $1/message ]; then
    printf "$1: \t%s\n" "`cat $1/message`"
fi
# check for bt script
if [ -r $1/script ]; then
	cd $1
    rm -f ${DBNAME}
	if [ "$2" = "create_output_masters" ]; then
		../../bt <script >output_master 2>&1
	else
		../../bt <script >output_test 2>&1
		diff output_master output_test >/dev/null 2>&1
		if [ $? -eq 0 -a -f ${DBNAME} ]; then
			printf "$1: \tpassed\n"
		else
			printf "$1: \tFAILED <--------------------------\n"
		fi
	fi
# check for shell script
elif [ -r $1/script.sh ]; then
	cd $1
	if [ "$2" = "create_output_masters" ]; then
		sh ./script.sh >output_master 2>&1
	else
		sh ./script.sh >output_test 2>&1
		diff output_master output_test >/dev/null 2>&1
		if [ $? -eq 0 ]; then
			printf "$1: \tpassed\n"
		else
			printf "$1: \tFAILED <--------------------------\n"
		fi    
    fi
else
	printf "$1: \tno script file\n"
fi
