#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "combine.h"

void combine1(vec_ptr v, int *dest);
void combine2(vec_ptr v, int *dest);
void combine3(vec_ptr v, int *dest);
void combine4(vec_ptr v, int *dest);

int vec_length(vec_ptr v);
long int *get_vec_start(vec_ptr v);
int get_vec_element (vec_ptr v, int index, long int *dest);
vec_ptr new_vec(int len);

int main() {

	long long c = 100000000; //10^8
	unsigned long curr, prev;
	struct timespec tstart, tfinish;
	double tsecs;

	vec_ptr myptr;
	int myresult;
	myptr = new_vec(30);

	clock_gettime(CLOCK_MONOTONIC, &tstart); // start time

	while(c-- >0){
		combine4(myptr, &myresult);
	}

	clock_gettime(CLOCK_MONOTONIC, &tfinish); //end time

	tsecs = (tfinish.tv_sec - tstart.tv_sec) + 1e-9 * (tfinish.tv_nsec - tstart.tv_nsec);

	printf("elapsed time = %f (sec) \n", tsecs);
	return 0;
}
//asm volatile("isb;mrs %0, cntvct_el0" : "=r"(prev));
//asm volatile("isb;mrs %0, cntvct_el0" : "=r"(curr));
