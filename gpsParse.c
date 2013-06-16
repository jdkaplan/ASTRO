#include <stdio.h>
#include <string.h>

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

char mul = 1;
char heightEnded = 0;
long int height;

int comma_count = 0;
int valid_gps;
gpsOut res = {0,0};
int chars_read = 0;

gpsOut gpsParse(char b) {
  res.ended = 0;
  // comma
  if (b == ',') {
    comma_count++;
  }
  else {
    switch (comma_count) {
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
        res.height = height*mul; break;// store
    }
  }
  if (++chars_read == HASP_LEN) {
    res.ended = 1;
    
    height = 0;
    heightEnded = 0;
    mul = 1;
    
    comma_count = 0;
    chars_read = 0;
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
  printf("%ld\n",out.height);
}

int main() {
  char gpsSend[] = ";;;;;\n";
  long int height;

  char input_string1[] = ";;1234470131.649,$GPGGA,202212.00,3024.7205,N,09110.7264,W,1,06,1.69,-99999.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  input_string1[0] = 0x1;
  input_string1[0] = 0x3;
  
  char input_string2[] = ";;1234470131.649,$GPGGA,202212.00,3024.7205,N,09110.7264,W,1,06,1.69,123456.9,M,-025,M,,*51,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  input_string2[0] = 0x1;
  input_string2[0] = 0x3;

  test(input_string1);
  test(input_string2);

  height = 123456;
  gpsSend[4] = (height%10) + '0';
  height /= 10;
  gpsSend[3] = (height%10) + '0';
  height /= 10;
  gpsSend[2] = (height%10) + '0';
  height /= 10;
  gpsSend[1] = (height%10) + '0';
  height /= 10;
  gpsSend[0] = (height%10) + '0';
  printf("%s", gpsSend);
}
