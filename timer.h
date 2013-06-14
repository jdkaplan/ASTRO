#define T_CLK (1000000l)
#define DIV (4l)
#define TICKS_PER_MS (T_CLK/(DIV*1000))

extern long int timerMS;

void timerStart();

