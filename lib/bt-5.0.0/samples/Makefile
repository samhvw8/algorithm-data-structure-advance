# Simple makefile for BT Library sample programs


TARGETS=${patsubst %.c,%,${wildcard ./*.c}}

default:	${TARGETS}

%:	%.c
	${CC} -o $@ $< -L../lib -lbt -I../inc

clean:
	rm -f ${TARGETS}

