#include <msp430.h>
#include "parsing.h"
#include "serial.h"
#include "hard.h"
#include "mainActions.h"

// Gets byte by byte the GPS string. Parses it, and maintains a state.
// When finished, it should return the reported height and flag saying the string
//   ended, in the gpsOut format.
// While not finished, it should return the ended flag as false (just return {0,0}).
// Note: global variable i contains the current byte's index (DO NOT CHANGE IT!).
// Note: HASP_LEN is the length of the string, including the two initial characters.

volatile char mul = 1;
volatile char heightEnded = 0;
volatile long height;

volatile int comma_count = 0;
volatile int valid_gps;
volatile gpsOut res = {0,0,0};
volatile int chars_read = 0;

volatile long time;
volatile long tau[8];
volatile int t = 0;

void resetGPS() {
  height = 0;
  heightEnded = 0;
  mul = 1;
  
  t = 0;

  comma_count = 0;
  chars_read = 0;  
}

gpsOut gpsParse(char b) {
  res.ended = 0;
  // comma
  if (b == ',') {
    comma_count++;
  }
  else {
    switch (comma_count) {
    case 2: // <1> block = timestamp
      if (b != '.') {
        tau[t++] = b - '0'; 
      }
      if (t == 8) {
        // my constants are probably wrong
        time = 3600000*tau[0] + 360000*tau[1] + // hour = 60 * 60 * 100
          60000*tau[2] + 6000*tau[3] +          // minute = 60 * 100
          1000*tau[4] + 100*tau[5] +            // second = 100
          10*tau[6] + 1*tau[7];                 // centisecond = 1
        res.timestamp = time;
      }
      break;
    case 7: // <6> block = fix indicator
      valid_gps = b - '0'; break;
    case 10: // <9> block = MSL altitude
      switch(b) {
      case '-':
	mul = -1;
	break;
      case '.':
	heightEnded = 1;
	break;
      default:
	height = heightEnded?height:(height*10 + (b-'0'));
      }
      break;
    case 11: // <9> block over
      if (valid_gps > 0)
        res.height = height*mul; // store
      else
	res.height = -1000;
      break;
    }
  }
  ++chars_read;
  if (chars_read == HASP_LEN) {
    res.ended = 1;
    resetGPS(); // TODO: remove
  }
  return res;
}

// Does the command indicated by the command byte.
void doCommand(char comm) {
  switch(comm) {
  case 2:
    sendLog(2);
    startOne(1);
    break;
  case 4:
    sendLog(4);
    startTwo(1);
    break;
  case 5:
    sendLog(5);
    // shutdown()
    break;
  case 0:
    sendLog(0);
    break;
  case 6:
    sendLog(6);
    startOne(0);
    break;
  case 7:
    sendLog(7);
  }
}
