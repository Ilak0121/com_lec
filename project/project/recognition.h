#define IMG_SIZE 784
#define IMG_COUNT 50000
#define DIGIT_COUNT 10

#include <time.h>

struct timespec mainS, mainE;
struct timespec recS, recE;

double sec1, sec2;
int sec1_count, sec2_count;


void recognition(float * images, float * network, int depth, int size, int * labels, float * confidences);

