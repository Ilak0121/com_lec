#define IMG_SIZE 784
#define IMG_COUNT 50000
#define DIGIT_COUNT 10
#include<arm_neon.h>

#include <time.h>

extern struct timespec mainS, mainE, forS;
extern struct timespec recS, recE, forE;

extern double sec1, sec2;
extern int sec1_count, sec2_count;

void recognition(float * images, float * network, int depth, int size, int * labels, float * confidences);


extern double for1_s, for2_s, for3_s;

