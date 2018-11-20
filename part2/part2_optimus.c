#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define PI  3.141592

typedef struct {
	double real, imag;
} COMPLEX;

FILE *fp;
struct timespec mainS, mainE, rdS, rdE, wrS, wrE, rdrecS, rdrecE, wrrecS, wrrecE, rddrecS, rddrecE;
struct timespec fftS, fftE, mylogS, mylogE;
double sec1, sec2, sec3, sec4, sec5, sec6, sec7, sec8;
int sec1_count,sec2_count,sec3_count,sec4_count,sec5_count,sec6_count,sec7_count,sec8_count;

FILE *open_read(char *);
FILE *open_write(char *);
void read_record(FILE *,double *,int);
void write_record(double *,int);
double *read_double_record(FILE *);

void fft(COMPLEX *,int);
int mylog2(unsigned int);

int main(void)
{
    clock_gettime(CLOCK_MONOTONIC, &mainS);
    int          i, length, fft_length, m;
    double       tempflt;
    double       *signal,*log_mag;
    COMPLEX      *samp;

    fp = open_read("test.txt");
    length = 128;
    signal = read_double_record(fp);

// determine fft size and allocate the complex array - fft size�� �迭
    m = mylog2(length);

    fft_length = 1 << m;   //bit shift
    samp = (COMPLEX *) calloc(fft_length, sizeof(COMPLEX));

// copy input signal to complex array and do the fft
    for (i=0; i<length; i++)
		samp[i].real = signal[i];

    fft(samp, m);

/* find log magnitude and store for output */
    log_mag = (double *)calloc(fft_length, sizeof(double));
    for (i=0; i< fft_length; i++)
    {
    	int temp1 = samp[i].real,temp2 = samp[i].imag;
    	tempflt  = temp1 * temp1;
    	tempflt += temp2 * temp2;
		log_mag[i] = 10 * log10(MAX(sqrt(tempflt), 1.e-14));
    	/*
        tempflt  = samp[i].real * samp[i].real;
        tempflt += samp[i].imag * samp[i].imag;
        tempflt =  sqrt(tempflt);
        log_mag[i] = 10 * log10(MAX(tempflt, 1.e-14));
        */
    }

    fp = open_write("test_out.txt");  //��� ���� �ؽ�Ʈ //
    write_record(log_mag, length);

    clock_gettime(CLOCK_MONOTONIC, &mainE);
    sec8 = (mainE.tv_sec - mainS.tv_sec) + 1e-9 * (mainE.tv_nsec - mainS.tv_nsec);
    sec8_count++;
    sec4-=sec3;//inner call time remove

    printf("%-20s %s : %.9lf (%d)\n","open_read", "[ms/call] (n called)",1000*sec1/sec1_count,sec1_count);
    printf("%-20s %s : %.9lf (%d)\n","open_write", "[ms/call] (n called)",1000*sec2/sec2_count,sec2_count);
    printf("%-20s %s : %.9lf (%d)\n","read_record", "[ms/call] (n called)",1000*sec3/sec3_count,sec3_count);
    printf("%-20s %s : %.9lf (%d)\n","read_double_record", "[ms/call] (n called)",1000*sec4/sec4_count,sec4_count);
    printf("%-20s %s : %.9lf (%d)\n","write_record", "[ms/call] (n called)",1000*sec5/sec5_count,sec5_count);
    printf("%-20s %s : %.9lf (%d)\n","fft", "[ms/call] (n called)",1000*sec6/sec6_count,sec6_count);
    printf("%-20s %s : %.9lf (%d)\n","log2", "[ms/call] (n called)",1000*sec7/sec7_count,sec7_count);
    printf("%-20s %s : %.9lf (%d)\n","main", "[ms/call] (n called)",1000*sec8/sec8_count,sec8_count);
    printf("%-20s %-20s : %.9lf\n","function total", "[ms]",1000*(sec1+sec2+sec3+sec4+sec5+sec6+sec7));

}

FILE *open_read(char *file_name) /* file name string */
{
    clock_gettime(CLOCK_MONOTONIC, &rdS);


    //fp = (FILE *) malloc(sizeof(FILE));


    fp = fopen(file_name,"r");

    clock_gettime(CLOCK_MONOTONIC, &rdE);

    sec1 += (rdE.tv_sec - rdS.tv_sec) + 1e-9 * (rdE.tv_nsec - rdS.tv_nsec);
    sec1_count ++;

    return(fp);
}

FILE *open_write(char * file_name)
{
	clock_gettime(CLOCK_MONOTONIC, &wrS);

    //fp = (FILE *) malloc(sizeof(FILE));


    fp = fopen(file_name,"wt");

    clock_gettime(CLOCK_MONOTONIC, &wrE);
    sec2 += (wrE.tv_sec - wrS.tv_sec) + 1e-9 * (wrE.tv_nsec - wrS.tv_nsec);
    sec2_count++;

    return(fp);
}

void read_record(FILE *fp ,double * input , int length)
{
	clock_gettime(CLOCK_MONOTONIC, &rdrecS);
    int status , i;
	for(i=0 ; i < length ; i++){
		//status = fscanf(fp,"%lf\n", &input[length + i]);
		status = fscanf(fp,"%lf\n", input+i); //�޸� ���� ���̱�
    }
	clock_gettime(CLOCK_MONOTONIC, &rdrecE);
    sec3 += (rdrecE.tv_sec - rdrecS.tv_sec) + 1e-9 * (rdrecE.tv_nsec - rdrecS.tv_nsec);
    sec3_count++;
}

double *read_double_record(FILE * fp)
{
    clock_gettime(CLOCK_MONOTONIC, &rddrecS);

    static double *buf;            /* input buffer to read data in */
    double *out;                    /* return output pointer */
    double *out_ptr;
	double *d_ptr;
    int i,length,length_in;

	length = 128;
    length_in = 256;

    //buf = (double *) calloc(length_in,sizeof(double));
    out = (double *) calloc(length,sizeof(double));


    read_record(fp , out , length);//buf , length);

     /*
     out_ptr = out;
     d_ptr = buf + 128;
     for(i = 0 ; i < length ; i++)
         *out_ptr++ = (double)(*d_ptr++);
     */

  clock_gettime(CLOCK_MONOTONIC, &rddrecE);
  sec4 += (rddrecE.tv_sec - rddrecS.tv_sec) + 1e-9 * (rddrecE.tv_nsec - rddrecS.tv_nsec);
  sec4_count++;

  return(out);
}

void write_record(double *ptr,int length)
{
    clock_gettime(CLOCK_MONOTONIC, &wrrecS);

	int status, i;
	for(i=0 ; i < length ; i++)
	{
		status = fprintf(fp,"%lf\n", *ptr++);
	}
    clock_gettime(CLOCK_MONOTONIC, &wrrecE);
    sec5 += (wrrecE.tv_sec - wrrecS.tv_sec) + 1e-9 * (wrrecE.tv_nsec - wrrecS.tv_nsec);
    sec5_count++;
}

void fft(COMPLEX *x, int m)
{
    clock_gettime(CLOCK_MONOTONIC, &fftS);

	static COMPLEX *w;           // used to store the w complex array
    static int mstore = 0;       // stores m for future reference
    static int n = 1;            // length of fft stored for future

    COMPLEX u, temp, tm;
    COMPLEX *xi, *xip, *xj, *wptr;

    int i, j, k, l, le, windex;

    double arg, w_real, w_imag, wrecur_real, wrecur_imag, wtemp_real;


    if(m != mstore)
	{

        if(mstore != 0) free(w);
        mstore = m;
        if(m == 0) return;       // if m=0 then done


        n = 1<<m;//(int)pow(2.0, m);				// ����ȭ ���
        le = n>>1;  //difference between the upper and lower leg indices			// ����ȭ ���

	// allocate the storage for w
        w = (COMPLEX *) calloc(le-1,sizeof(COMPLEX));

	 // calculate the w values recursively ���
        arg = PI/le;         //  PI/le calculation
        wrecur_real = w_real = cos(arg);
        wrecur_imag = w_imag = -sin(arg);
        xj = w;
        for (j = 1 ; j < le ; j++)
		{
            xj->real = (double)wrecur_real;
            xj->imag = (double)wrecur_imag;
            xj++;
            wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
            wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
            wrecur_real = wtemp_real;
        }
    }

 // start fft Ǫ���� ��ȯ ����
    le = n;
    windex = 1;
    for (l = 0 ; l < m ; l++)
	{
        le = le>>1;							// ����ȭ ���
  // first iteration with no multiplies
        for(i = 0 ; i < n ; i = i + le<<1)
		{
            xi = x + i;
            xip = xi + le;
            double real = xi->real, imag=xi->imag;
            double realp = xip->real, imagp=xip->imag;
            temp.real = real + realp;
            temp.imag = imag + imagp;
            xip->real = real - realp;
            xip->imag = imag - imagp;
            *xi = temp;
        }
  // remaining iterations use stored w
        wptr = w + windex - 1;
        for (j = 1 ; j < le ; j++)
		{
            u = *wptr;
            for (i = j ; i < n ; i = i + le<<1)//2*le)
			{
                xi = x + i;
                xip = xi + le;
                double real = xi->real, imag=xi->imag;
				double realp = xip->real, imagp=xip->imag;
                temp.real = real + realp;
                temp.imag = imag + imagp;
                tm.real = real - realp;
                tm.imag = imag - imagp;
                xip->real = tm.real*u.real - tm.imag*u.imag;
                xip->imag = tm.real*u.imag + tm.imag*u.real;
                *xi = temp;
            }
            wptr = wptr + windex;
        }
        windex = windex<<1;//2*windex;
    }
 // rearrange data by bit reversing
    j = 0;
    for (i = 1 ; i < (n-1) ; i++)
	{
		k=n>>1;//k = n/2;
        while(k <= j)
		{
            j = j - k;
            k=k>>1;//k = k/2;
        }
		j=j+k;
        if (i < j)
		{
            xi = x + i;
            xj = x + j;
            temp = *xj;
            *xj = *xi;
            *xi = temp;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &fftE);
    sec6 += (fftE.tv_sec - fftS.tv_sec) + 1e-9 * (fftE.tv_nsec - fftS.tv_nsec);
    sec6_count++;

}

int mylog2(unsigned int x)
{
    clock_gettime(CLOCK_MONOTONIC, &mylogS);

    unsigned int mask,i;

    if(x == 0) return(-1);      // zero is an error, return -1
    x--;                        // get the max index, x-1
    for(mask = 1 , i = 0 ; ; mask <<= 1 , i++)//mask *= 2 , i++)
	{
        if(x == 0){
            clock_gettime(CLOCK_MONOTONIC, &mylogE);
            sec7 += (mylogE.tv_sec - mylogS.tv_sec) + 1e-9 * (mylogE.tv_nsec - mylogS.tv_nsec);
            sec7_count++;
        	return(i);   // return log2 if all zero
        }
        x = x & (~mask);        // AND off a bit
    }

}
