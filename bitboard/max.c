#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void main()
{
	srand( time( NULL ) );
	
	int16_t m = INT16_MIN;
	for (int i = 0; i < 10; i++) {
		int16_t a = rand();
		
		// if (m < a) {
			// m = a;
		// }
		
		m = MAX(a, m);
		
		printf("%d\n", a);
	}
	
	printf("m:%d\n", m);
}