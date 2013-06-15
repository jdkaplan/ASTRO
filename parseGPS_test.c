//NOTE: WILL ONLY BE ACCURATE TO WITHIN A METER BECAUSE USE OF LONGS INSTEAD OF DOUBLES


#include <stdio.h>
#include <stdlib.h>

#define HASP_LEN 122

typedef struct {
  long int height;
  char ended;
} gpsOut;

// Gets byte by byte the GPS string. Parses it, and maintains a state.
// When finished, it should return the reported height and flag saying the string
//   ended, in the gpsOut format.
// While not finished, it should return the ended flag as false (just return {0,0}).
// Note: global variable i contains the current byte's index (DO NOT CHANGE IT!).
// Note: HASP_LEN is the length of the string, including the two initial characters.

char gps_height []= ";;;;;;;"; // because range is -9999.9 to 99999.9 (7 chars)
int digit = 0;
int comma_count = 0;
int valid_gps;
gpsOut res = {0,0};
int chars_read = 0;

gpsOut gpsParse(char b) {
  // comma
  if (b == ',') {
    comma_count++;
  }
  else {
    switch (comma_count) {
    case 7: // <6> block = fix indicator
      valid_gps = b - '0'; break;
    case 10: // <9> block = MSL altitude
      gps_height[digit++] = b; break;
    case 11: // <9> block over
      if (valid_gps > 0)
        res.height = strtol(gps_height,NULL,10); break;// store
    }
  }
  if (chars_read++ == HASP_LEN) {
    res.ended = 1;
    char gps_height [] = ";;;;;;;"; 
    digit = 0;
    comma_count = 0;
    chars_read = 0;
  }
  return res;
}


// TESTING
int main() {
  char *input_string = "1234470131.649,$GPGGA,202212.00,3024.7205,N,09110.7264,W,1,06,1.69,-9999.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  
  int j;
  gpsOut out;
  for (j=0; j<HASP_LEN; j++) {
    out = gpsParse(input_string[j]);
  }
  printf(gps_height);
  printf("\n");
  printf("%ld",out.height);
  printf("\n");
}
