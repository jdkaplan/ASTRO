char messageStarted = 0;
unsigned char first;

void parseByte() {
  // if recv/parse queues collide, nothing to read
  if (inpSel == inputTop) {
    return;
  }
  
  // get the character from input buffer and increment/wrap
  char b = inputBuffer[inpSel];
  inpSel = (inpSel+1) % LEN_INP_BUF;
  
  // message hasn't started
  if (messageStarted == 0) {
    // valid start bytes are 0x0 thru 0x9 except for 0x3
    if (b <= 0x09 && b != 0x03) {
      messageStarted = 1;
      first = b;
    }
    // invalid start byte
    else {
      // pass
    }
  }

  // expecting second byte of message
  else if (messageStarted == 1) {
    // expecting HASP data, 0x30 is second byte of HASP string
    if (first == 0x01 && b == 0x30) {
      messageStarted = 2;
    }
    // expecting command, commands are pairs of identical bytes
    else if (b == first) {
      messageStarted = 0;
      doCommand(b);
    }
    // invalid second byte
    else {
      messageStarted = 0;
    }
  }
  
  // parsing GPS stuff
  else if (messageStarted == 2) {
    // run it through the gpsParse state machine
    gpsOut g = gpsParse(b);
    
    // b was the last byte expected by gpsParse
    if (g.ended) {
      // positive altitude!
      if (g.height >= 0) {
        // store it!
        globalState.height = g.height;
      }
      // because I am a clock
      globalState.externalTime = g.timestamp;
      // pong
      doCommand(0);
      messageStarted = 3;
    }
    
    // gps string hasn't ended yet
    else {
      // pass
    }
  }
  
  // expecting tail of HASP data
  else if (messageStarted == 3) {
    // tail consists of 0x030D0A
    if (b == 0x03 || b == 0x0D || b == 0x0A) {
      messageStarted = 0;
    }
    // invalid tail
    else {
      messageStarted = 0;
    }
  }
  
  else {
    // How the fuck did you get here?
  }
}
