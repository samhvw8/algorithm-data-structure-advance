CC=gcc
FLAGS = -Wall -Werror -Wextra -Warray-bounds -Wnonnull
CFLAGS= -c $(FLAGS)
LDFLAGS= $(FLAGS)
LIB = ./
SRCS=test.c jval.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=test
another= gmon.out

all: $(SRCS) $(EXECUTABLE)



debug: CFLAGS += -g
debug: LDFLAGS += -g
debug:  $(SRCS) $(EXECUTABLE)

profiling: CFLAGS += -pg
profiling: LDFLAGS += -pg
profiling: all

$(EXECUTABLE): $(OBJS)
	$(CC) -I $(LIB) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) -I $(LIB) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE) $(another)

remake: clean all
redebug: clean debug
reprofiling: clean profiling

