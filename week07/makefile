CC=gcc
FLAGS = -Wall -Wextra -Warray-bounds -Wnonnull
CFLAGS= -c $(FLAGS)
LDFLAGS= $(FLAGS)
INC =../lib/inc 
LIB =../lib/llib
LFLAGS = -lfdr -lutility
SRCS=graph.c metro_stations.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=metro_stations
another= gmon.out

all: $(SRCS) $(EXECUTABLE)

.PHONY: clean

debug: clean
debug: CFLAGS += -g
debug: LDFLAGS += -g
debug: all

prof: clean
prof: CFLAGS += -pg
prof: LDFLAGS += -pg
prof: all

udgraph: clean
udgraph: CFLAGS += -DUDGGIN
udgraph: all

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ -I$(INC) -L$(LIB) $(LFLAGS)

.c.o:
	$(CC) -I $(INC) $(CFLAGS) $< -c

clean:
	rm -rf *.o $(EXECUTABLE) $(another)

remake: clean all


