#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <arm_neon.h>

struct timeval tstart, tfinish;
double tsecs;

int main(void){

	
	int a,b,c;
	float A[256][256];
	float B[256][256];
	float C[256][256];
	float Result = 0;
	float temp = 0;
	float32x4_t Avec, Bvec, Cvec;

	
	// Matrix Initialization
	for (a=0; a<256; a++){
		for (b=0; b<256; b++){
			A[a][b] = a+b;
			B[a][b] = 0.01;
			C[a][b] = 0;
		}
	}
	

	// Matrix multiplicatio
	// Warm up
	#pragma omp parallel for private(b,c)
	for (a=0; a<128; a++){
		for (b=0; b<128; b++){
			for (c=0; c<128; c++){
				temp = A[c][b] * B[a][c];
			}
		}
	}

	gettimeofday(&tstart, NULL);
	#pragma omp parallel for private(a,b,c, Avec, Bvec, Cvec)
	for (a=0; a<256; a++){
		for (b=0; b<256; b++){
			//C[a][b] += A[a][c] * B[c][b];
			Cvec = vdupq_n_f32(0);
			for (c=0; c<256; c+=4){
				Avec = vld1q_f32(&A[a][c]);
				Bvec = vld1q_lane_f32(&B[c][b], Bvec, 0);
				Bvec = vld1q_lane_f32(&B[c+1][b], Bvec, 1);
				Bvec = vld1q_lane_f32(&B[c+2][b], Bvec, 2);
				Bvec = vld1q_lane_f32(&B[c+3][b], Bvec, 3);
				Cvec = vmlaq_f32(Cvec, Avec, Bvec); 
			}
			C[a][b] = vgetq_lane_f32(Cvec, 0);
			C[a][b] += vgetq_lane_f32(Cvec, 1);
			C[a][b] += vgetq_lane_f32(Cvec, 2);
			C[a][b] += vgetq_lane_f32(Cvec, 3);
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
