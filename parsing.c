#include <msp430.h>
#include "parsing.h"
#include "serial.h"
#include "hard.h"
#include "mainActions.h"
#include "state.h"
#include "motors.h"

#define START_ATOMIC() __bic_SR_register(GIE)
#define END_ATOMIC() __bis_SR_register(GIE)


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

int checksum = 0;
short checking = 0;

void resetGPS() {
  height = 0;
  heightEnded = 0;
  mul = 1;
  
  t = 0;

  comma_count = 0;
  chars_read = 0;  

  checking = 0;
}

gpsOut gpsParse(char b) {
  res.ended = 0;

  // checksum calculation between $ and * exclusive
  // logic is backwards for a good reason (byte-by-byte parsing)

  // second character of checksum
  if (checking == 3) {
    checksum += b - '0';
    res.checkedsum = (checksum == res.checkedsum) ? 1 : 0;
    res.ended = 1;
    resetGPS();
  }
  // first character of checksum
  if (checking == 2) {
    checksum = 0x10 * (b - '0');
    checking = 3;
  }
  // stop calculating, start reading included checksum
  if (b == '*') {
    checking = 2;
  }
  // calculate checksum
  if (checking == 1) {
    res.checkedsum ^= b;
  }
  // start calculating checksum
  if (b == '$') {
    checking = 1;
    res.checkedsum = 0;
  }

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
  return res;
}

// Does the command indicated by the command byte.
void doCommand(char comm) {
  switch(comm) {
  case 0x00:
    sendLog(0x00);
    break;

  // 0x01 is reserved for HASP data

  case 0x02:
    START_ATOMIC();
    globalState.safemode = 1;
    END_ATOMIC();

    sendLog(0x02);
    break;

  // 0x03 is reserved for HASP data

  case 0x04:
    turnOne(OPEN);
    sendLog(0x04);
    break;

  case 0x05:
    turnOne(CLOSE);
    sendLog(0x05);
    break;

  case 0x06:
    turnTwo(OPEN);
    sendLog(0x06);
    break;

  case 0x07:
    turnTwo(CLOSE);
    sendLog(0x07);
    break;

  case 0x08:
    turnHVDC1On();
    sendLog(0x08);
    break;

  case 0x09:
    turnHVDC1Off();
    sendLog(0x09);
    break;

  // 0x0A reserved for HASP data

  case 0x0B:
    turnHVDC2On();
    sendLog(0x0B);
    break;

  case 0x0C:
    turnHVDC2Off();
    sendLog(0x0C);
    break;

  // 0x0D reserved for HASP data

  case 0x0E:
    turnHeater1On();
    sendLog(0x0E);
    break;

  case 0x0F:
    turnHeater1Off();
    sendLog(0x0F);
    break;

  case 0x10:
    turnHeater2On();
    sendLog(0x10);
    break;

  case 0x11:
    turnHeater2Off();
    sendLog(0x11);
    break;

  case 0x12:
    // TODO: shutdown command
    sendLog(0x12);
    break;

  case 0x13:
    START_ATOMIC();
    globalState.safemode = 0;
    END_ATOMIC();
    sendLog(0x13);
    break;
  }

  /* case 0x14: */
  /*   sendLog(0x14); */
  /*   break; */

  /* case 0x15: */
  /*   sendLog(0x15); */
  /*   break; */

  /* case 0x16: */
  /*   sendLog(0x16); */
  /*   break; */

  /* case 0x17: */
  /*   sendLog(0x17); */
  /*   break; */

  /* case 0x18: */
  /*   sendLog(0x18); */
  /*   break; */

  /* case 0x19: */
  /*   sendLog(0x19); */
  /*   break; */
  /* } */
}
