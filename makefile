SOURCES = main.c motors.c parsing.c serial.c timer.c

main: $(SOURCES)
	gcc $(SOURCES) -o main.o
