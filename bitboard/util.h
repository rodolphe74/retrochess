#ifndef _TIME_
#define _TIME_

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void tstart();
void tstop(char *label);
void str_tstop(char *result);

#endif
