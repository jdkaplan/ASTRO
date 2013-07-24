#include <stdio.h>
#include <string.h>

#define HASP_LEN 120

typedef struct {
  long int height;
  long int timestamp;
  char ended;
} gpsOut;

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
    resetGPS();
  }
  return res;
}

// TESTING
void test(char *input_string) {
  int j;
  gpsOut out;
  for (j=0; j<strlen(input_string); j++) {
    out = gpsParse(input_string[j]);
  }
  if(!out.ended) {
    printf("DIE DIE DIE\n");
  }
  printf("%ld ",out.height);
  printf("%ld ",out.timestamp);
  long a = ((out.timestamp>>24)     );
  long b = ((out.timestamp>>16)&0xFF);
  long c = ((out.timestamp>> 8)&0xFF);
  long d = ((out.timestamp    )&0xFF);
  printf("(%x, ", a);
  printf( "%x, ", b);
  printf( "%x, ", c);
  printf( "%x) " , d);
  long k = (a<<24) + (b<<16) + (c<<8) + d;
  long l = a*0x1000000 + b*0x10000 + c*0x100 + d;
  printf("%ld ", k);
  printf("%ld ", l);
  printf("\n");
}

int main() {
  char gpsSend[] = ";;;;;\n";
  long int height;

  char input_string1[] = "1234470131.649,$GPGGA,000000.00,3024.7205,N,09110.7264,W,1,06,1.69,-99999.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  input_string1[0] = 0x1;
  input_string1[0] = 0x3;
  
  char input_string2[] = "1234470131.649,$GPGGA,185304.20,3024.7205,N,09110.7264,W,1,06,1.69,123456.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";

  char input_string3[] = "1234470131.649,$GPGGA,235959.99,3024.7205,N,09110.7264,W,1,06,1.69,999999.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";

  test(input_string3);
  test(input_string3);
  test(input_string3);
  test(input_string3);
  test(input_string3);
  test(input_string3);
  test(input_string3);
}
