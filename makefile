SOURCES = main.c motors.c parsing.c serial.c timer.c

main: $(SOURCES)
	msp430-gcc -mmcu=msp430f2274 $(SOURCES) -o main.o
