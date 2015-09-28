CC=gcc
FLAGS = -Wall -Wextra -Warray-bounds -Wnonnull
CFLAGS= -c $(FLAGS)
LDFLAGS= $(FLAGS)
INC = ../lib/inc/
LIB = ../lib/llib
LFLAGS = -lbt -lutility
SRCS=test_lib.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=test_lib
another= gmon.out gui

all: $(SRCS) $(EXECUTABLE)

debug: CFLAGS += -g
debug: LDFLAGS += -g
debug:  $(SRCS) $(EXECUTABLE)

profiling: CFLAGS += -pg
profiling: LDFLAGS += -pg
profiling: all

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ -I$(INC) -L$(LIB) $(LFLAGS)

gui:
	gcc -o gui gui.c -L$(LIB) -I$(INC) -lbt `pkg-config --cflags --libs gtk+-3.0`
.c.o:
	$(CC) -I $(INC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE) $(another)

remake: clean all
redebug: clean debug
reprofiling: clean profiling

