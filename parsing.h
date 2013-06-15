#ifndef PARSING_H
#define PARSING_H

#define HASP_LEN 122

typedef struct {
  long int height;
  char ended;
} gpsOut;

gpsOut gpsParse(char);
void doCommand(char);

#endif
