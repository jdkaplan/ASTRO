#ifndef PARSING_H
#define PARSING_H

#define HASP_LEN 120

typedef struct {
  long int height;
  long int timestamp;
  char ended;
  int checkedsum;
} gpsOut;

gpsOut gpsParse(char);
void doCommand(char);
void resetGPS();

#endif
