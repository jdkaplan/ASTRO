#include <stdio.h>
#include <string.h>

#define HASP_LEN 120

typedef struct {
  long int height;
  long int timestamp;
  char ended;
  int checkedsum;
} gpsOut;

volatile char mul = 1;
volatile char heightEnded = 0;
volatile long height;

volatile int comma_count = 0;
volatile int valid_gps;
volatile gpsOut res = {0,0,0,0};
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
  // logic is for a good reason

  // second character of checksum
  if (checking == 3) {
    char val = (b>'9')?(((b>'F')?(b-'a'):(b-'A'))+0xA):(b-'0');
    checksum += val;
    res.checkedsum = (checksum == res.checkedsum) ? 1 : 0;
    res.ended = 1;
    resetGPS();
  }
  // first character of checksum
  if (checking == 2) {
    char val = (b>'9')?(((b>'F')?(b-'a'):(b-'A'))+0xA):(b-'0');
    checksum = 0x10 * val;
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
  /* if (b == '$') { */
  if ((comma_count == 0) && (b == ',')) {
    checking = 1;
    res.checkedsum = 0;
  }

  // comma
  if (b == ',') {
    comma_count++;
    if(comma_count == 11) { // <9> block over
      if (valid_gps > 0)
        res.height = height*mul; // store
      else
        res.height = -1000;
    }

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
    }
  }
  return res;
}

// TESTING
int check(char *s) {
  int c = 0;
 
  while(*s)
    c ^= *s++;
 
  return c;
}

void test(char *input_string) {
  int j;
  gpsOut out;
  for (j=0; j<strlen(input_string); j++) {
    out = gpsParse(input_string[j]);
  }
  if(!out.ended) {
    printf("DIE DIE DIE\n");
  }
  printf("Height: %ld\n",out.height);
  printf("Time: %ld\n",out.timestamp);
  printf("Check: %s\n\n",out.checkedsum ? "true" : "false");
  resetGPS();
}

int main() {
  //char input_string1[] = "1234470131.649,$GPGGA,000000.00,3024.7205,N,09110.7264,W,1,06,1.69,-99999.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  //char input_string2[] = "1234470131.649,$GPGGA,185304.20,3024.7205,N,09110.7264,W,1,06,1.69,123456.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  //char input_string3[] = "1234470131.649,$GPGGA,235959.99,3024.7205,N,09110.7264,W,1,06,1.69,999999.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  char input_string4[] = "1234470131.649,GPGGA,202212.00,3024.7205,N,09110.7264,W,1,06,1.69,999999.9,M,-025,M,,*71";
  char input_string5[] = "1234567890.098,GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
  char input_string6[] = "1374772186.61,GPGGA,235959.99,,,,,1,,,10000,,,,,*79";
  char input_string7[] = "1,GPGGA,002000.00,,,,,1,,,1100,,,,,*4b";
  char input_string8[] = "0,GPGGA,165826.00,3146.7630,N,09543.0260,W,1,05,1.90,00141,M,-024,M,,*5F";

  test(input_string4);
  test(input_string5);
  test(input_string6);
  test(input_string7);
  test(input_string8);
  return 0;
}
