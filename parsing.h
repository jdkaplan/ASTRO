#ifndef PARSING_H
#define PARSING_H

#define HASP_LEN 120

typedef struct {
  long int height;
  long int timestamp;
  char ended;
} gpsOut;

gpsOut gpsParse(char);
void doCommand(char);

#endif
