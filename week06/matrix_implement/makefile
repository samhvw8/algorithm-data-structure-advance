CC=gcc
FLAGS = -Wall -Wextra -Warray-bounds -Wnonnull
CFLAGS= -c $(FLAGS)
LDFLAGS= $(FLAGS)
INC = 
LIB =
LFLAGS = 
SRCS=graph.c test_graph.c 
OBJS=$(SRCS:.c=.o)
EXECUTABLE=test_graph
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
	$(CC) -I $(INC) $(CFLAGS) $< -c 

clean:
	rm -rf *.o $(EXECUTABLE) $(another)

remake: clean all
redebug: clean debug
reprofiling: clean profiling

