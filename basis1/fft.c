#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define PI  3.141592

typedef struct {
    double real, imag;
} COMPLEX;

   FILE *fp;
 
/* function prototypes for DSP disk file functions */ 
   FILE *open_read(char *);
   FILE *open_write(char *);
   void read_record(FILE *,double *,int);
   void write_record(double *,int);
   double *read_double_record(FILE *);

/* function prototypes for dft and inverse dft functions */
   void fft(COMPLEX *,int);
   int log2(unsigned int);

/************************************************************************
IFFTTEST.C - Demonstrate and test FFT and Inverse FFT functions
Requires time domain data file in DSP_FILE format.
Generates DSP_FILE format files for spectral magnitude
and reconstructed time domain data.
************************************************************************/

int main(void)
{
    int          i, length, fft_length, m;
    double       tempflt;
    double       *signal,*log_mag;
    COMPLEX      *samp;

// Read the input data file from the dsp format.  ����Ÿ���� �д� ���
    fp = open_read("test.txt");
    length = 128;
    signal = read_double_record(fp);

// determine fft size and allocate the complex array - fft size�� �迭
    m = log2(length); 
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
        tempflt  = samp[i].real * samp[i].real;
        tempflt += samp[i].imag * samp[i].imag;
        tempflt =  sqrt(tempflt);
        log_mag[i] = 10 * log10(MAX(tempflt, 1.e-14));
    }
    fp = open_write("test_out.txt");  //��� ���� �ؽ�Ʈ //
    write_record(log_mag, length);
    return 0;
}

/**************************************************************************
DISKIO.C - Source code for DSP data format read and write functions
    open_read          open DSP data file to be read
    open_write         create header and open DSP data file for write
    read_record        read one record
    write_record       write one record
    read_float_record  read one record and convert to float array
*************************************************************************/

/***************************************************************************************
open_read - open a DSP data file for read
Returns a pointer to a DSP_FILE structure allocated by the function and opens file_name.
Allocation errors or improper type causes a call to exit(1).
A bad file_name returns a NULL pointer.
DSP_FILE *open_read(char *file_name)
***************************************************************************************/

FILE *open_read(char *file_name) /* file name string */
{
/* allocate the DSP data file structure */

    fp = (FILE *) malloc(sizeof(FILE));

/* open file for text read and update */
    fp = fopen(file_name,"r"); 
    return(fp);
}

/**************************************************************************
open_write - open a DSP data file for write
Returns a pointer to a DSP_FILE structure allocated by the function.
Allocation errors or improper type causes a call to exit(1).
A bad file name returns a NULL pointer.
DSP_FILE *open_write(char *file_name, int records,int rec_len)
    file_name       pointer to file name string
    records         number of records of data to be written
    rec_len         number of elements in each record
*************************************************************************/

FILE *open_write(char * file_name)
{
/* allocate the DSP data file structure */
    fp = (FILE *) malloc(sizeof(FILE));

/* open file for text write and update*/
    fp = fopen(file_name,"wt");
    return(fp);
}

/********************************************************************
read_record - read one record of DSP data file
Exits if a read error occurs or if the DSP_FILE structure is invalid.    
Void read_record(FILE *fp , double *input , int length)
********************************************************************/

void read_record(FILE *fp ,double * input , int length)
{
    int status , i;
	for(i=0 ; i < length ; i++)
	{
		status = fscanf(fp,"%lf\n", &input[length + i]);
    }
}

/************************************************************************************************************************
read_double_record - read one record of DSP data file and convert to float array of values.
Returns a pointer to the beginning of the allocated float array of values representing the record read from the DSP_FILE.
Exits if a read or allocation error occurs.
float *read_double_record(DSP_FILE *dsp_info)
************************************************************************************************************************/

double *read_double_record(FILE * fp)
{
    static double *buf;            /* input buffer to read data in */
    double *out;                    /* return output pointer */
    double *out_ptr;
	double *d_ptr;
    int i,length,length_in;

	length = 128;
    length_in = 256;
 
    buf = (double *) calloc(length_in,sizeof(double));
    out = (double *) calloc(length,sizeof(double));

/* read the record into buf */
    read_record(fp , buf , length);

/* perform conversion to floating point */
     out_ptr = out;  
     d_ptr = buf + 128;
     for(i = 0 ; i < length ; i++)
         *out_ptr++ = (double)(*d_ptr++);
  
  return(out);                 /* return converted pointer */
}

/**************************************************************************
write_record - write one record of DSP_FILE data
Exits if write error occurs or if the DSP_FILE structure is invalid.
void write_record(char *ptr,DSP_FILE *dsp_info, int length)
    ptr        pointer to data to write to disk (type in dsp_info)
    dsp_info   pointer to DSP data file structure    
*************************************************************************/

void write_record(double *ptr,int length)
{
	int status, i;
	for(i=0 ; i < length ; i++)
	{
		status = fprintf(fp,"%lf\n", *ptr++);
	}	
}

/**********************************************************************
DFT.C - SOURCE CODE FOR DISCRETE FOURIER TRANSFORM FUNCTIONS
fft     In-place radix 2 decimation in time FFT
log2    Base 2 logarithm
***********************************************************************/

/**********************************************************************
fft - In-place radix 2 decimation in frequency FFT
Requires pointer to complex array, x and power of 2 size of FFT, m
(size of FFT = 2^m).  Places FFT output on top of input COMPLEX array.
void fft(COMPLEX *x, int m)
***********************************************************************/

void fft(COMPLEX *x, int m)
{  
	static COMPLEX *w;           // used to store the w complex array 
    static int mstore = 0;       // stores m for future reference 
    static int n = 1;            // length of fft stored for future 

    COMPLEX u, temp, tm;
    COMPLEX *xi, *xip, *xj, *wptr;

    int i, j, k, l, le, windex;

    double arg, w_real, w_imag, wrecur_real, wrecur_imag, wtemp_real;

	
    if(m != mstore) 
	{
	// free previously allocated storage and set new m 
        if(mstore != 0) free(w);
        mstore = m;
        if(m == 0) return;       // if m=0 then done 

	// n = 2^m = fft length 
        n = (int)pow(2.0, m);				// ����ȭ ���
        le = n/2;  //difference between the upper and lower leg indices			// ����ȭ ���

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
        le = le/2;							// ����ȭ ���
  // first iteration with no multiplies 
        for(i = 0 ; i < n ; i = i + 2*le) 
		{
            xi = x + i;
            xip = xi + le;
            temp.real = xi->real + xip->real;
            temp.imag = xi->imag + xip->imag;
            xip->real = xi->real - xip->real;
            xip->imag = xi->imag - xip->imag;
            *xi = temp;
        }
  // remaining iterations use stored w 
        wptr = w + windex - 1;
        for (j = 1 ; j < le ; j++) 
		{
            u = *wptr;
            for (i = j ; i < n ; i = i + 2*le) 
			{
                xi = x + i;
                xip = xi + le;
                temp.real = xi->real + xip->real;
                temp.imag = xi->imag + xip->imag;
                tm.real = xi->real - xip->real;
                tm.imag = xi->imag - xip->imag;             
                xip->real = tm.real*u.real - tm.imag*u.imag;
                xip->imag = tm.real*u.imag + tm.imag*u.real;
                *xi = temp;
            }
            wptr = wptr + windex;
        }
        windex = 2*windex;
    }
 // rearrange data by bit reversing 
    j = 0;
    for (i = 1 ; i < (n-1) ; i++) 
	{
		k = n/2;
        while(k <= j)					
		{
            j = j - k;					
            k = k/2;					
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


}

/**************************************************************************
log2 - base 2 logarithm
Returns base 2 log such that i = 2^ans where ans = log2(i).
if log2(i) is between two values, the larger is returned.
int log2(unsigned int x)
*************************************************************************/
int log2(unsigned int x)
{
    unsigned int mask,i;

    if(x == 0) return(-1);      // zero is an error, return -1 
    x--;                        // get the max index, x-1 
    for(mask = 1 , i = 0 ; ; mask *= 2 , i++) 
	{
        if(x == 0) return(i);   // return log2 if all zero 
        x = x & (~mask);        // AND off a bit 
    }

}

