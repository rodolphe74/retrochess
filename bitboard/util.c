#include "util.h"
#include <time.h>
#include <stdio.h>


static clock_t t;
static double time_taken;

void tstart()
{
	t = clock();
}

void tstop(char *label)
{
	t = clock() - t;
	time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
	printf("temps %s :  %f sec [%lu] %c%c", label, time_taken, CLOCKS_PER_SEC, 10, 13);
}

void str_tstop(char *result)
{
	t = clock() - t;
	time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
	sprintf(result, "%f", time_taken);
}

double f_tstop()
{
	t = clock() - t;
	return ((double)t) / CLOCKS_PER_SEC; 
}
