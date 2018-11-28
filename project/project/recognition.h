#define IMG_SIZE 784
#define IMG_COUNT 50000
#define DIGIT_COUNT 10

#include <time.h>

extern struct timespec mainS, mainE;
extern struct timespec recS, recE;

extern double sec1;
extern double sec2;
extern int sec1_count;
extern int sec2_count;


void recognition(float * images, float * network, int depth, int size, int * labels, float * confidences);

