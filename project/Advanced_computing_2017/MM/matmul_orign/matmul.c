//#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct timeval tstart, tfinish;
double tsecs;

int main(void){

	
	int a,b,c;
	float A[256][256];
	float B[256][256];
	float C[256][256];
	float Result = 0;
	
	// Matrix Initialization
	for (a=0; a<256; a++){
		for (b=0; b<256; b++){
			A[a][b] = a+b;
			B[a][b] = 0.01;
			C[a][b] = 0;
		}
	}
	

	// Matrix multiplication

	gettimeofday(&tstart, NULL);
	for (a=0; a<256; a++){
		for (b=0; b<256; b++){
			for (c=0; c<256; c++){
				C[a][b] += A[a][c] * B[c][b];
			}
		}
	}
	gettimeofday(&tfinish, NULL);
	tsecs = (tfinish.tv_sec - tstart.tv_sec) + 0.000001 * (tfinish.tv_usec - tstart.tv_usec);
	printf("Matrix multiplication> Elapsed time : %2f sec\n",  tsecs);

	// Check the results
	for (a=0; a<256; a++){
		for (b=0; b<256; b++){
			Result += C[a][b];
		}
	}
	Result /= 256*256;
	printf("Matrix multiplication> Result : %6f\n", Result);


}
