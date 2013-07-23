CC=msp430-gcc
CFLAGS=-c -Wall
LDFLAGS=-mmcu=msp430f2274
SOURCES=motors.c timer.c parsing.c serial.c main.c hard.c mainActions.c state.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=derpacopter

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
