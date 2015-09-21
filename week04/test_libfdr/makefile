CC=gcc
FLAGS = -Wall -Werror -Wextra -Warray-bounds -Wnonnull
CFLAGS= -c $(FLAGS)
LDFLAGS= $(FLAGS)
INC = ../../lib/inc/
LIB = ../../lib/llib
LFLAGS = -lfdr -lutility
SRCS=test_lib.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=test_lib
another= gmon.out

all: $(SRCS) $(EXECUTABLE)

debug: CFLAGS += -g
debug: LDFLAGS += -g
debug:  $(SRCS) $(EXECUTABLE)

profiling: CFLAGS += -pg
profiling: LDFLAGS += -pg
profiling: all

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ -I$(INC) -L$(LIB) $(LFLAGS)

.c.o:
	$(CC) -I $(INC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE) $(another)

remake: clean all
redebug: clean debug
reprofiling: clean profiling

