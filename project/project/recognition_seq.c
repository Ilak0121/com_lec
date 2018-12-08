#include <stdio.h>
#include <stdlib.h>
#include "recognition.h"
#include <math.h>

#define DEGREE 4

#define sigmoid(x) (1 / (1 + exp(-x)))

void recognition(float * images, float * network, int depth, int size, int * labels, float * confidences)
{

   clock_gettime(CLOCK_MONOTONIC, &recS);

  int i, j, x, y;
  float *hidden_layers, **weights, **biases; //temp delete 

  hidden_layers = (float *)malloc(sizeof(float) * size * depth);//4*64*2 in small net
  weights = (float **)malloc(sizeof(float *) * (depth + 1));
  biases = (float **)malloc(sizeof(float *) * (depth + 1));

  // Set pointers for weights and biases
  // 1. Input layer
  weights[0] = network;
  biases[0] = weights[0] + size * IMG_SIZE;
  // 2. Hidden layers
  for(i = 1; i < depth; i++)
  {
    weights[i] = network + (size * IMG_SIZE + size) + (size * size + size) * (i-1);
    biases[i] = weights[i] + size * size;
  }
  // 3. Output layer
  weights[depth] = weights[depth - 1] + size * size + size;
  biases[depth] = weights[depth] + DIGIT_COUNT * size;

  //float sum =0;
  float32x4_t sum;//change to neon
  float32x4_t Avec,Bvec;

  float max = 0;
  int label = 0;

  float *input;
  float output[DIGIT_COUNT];

  int IS_X;
  
  float* data = (float*)calloc(size,sizeof(float));
  for(y=0; y<size; y++)
      data[y] =0;

  // Recognize numbers
  for(i = 0; i < IMG_COUNT; i++)
  {
    input = images + IMG_SIZE * i;

    // From the input layer to the first hidden layer
    //clock_gettime(CLOCK_MONOTONIC,&forS);

    for(x = 0; x < size; x++)
    {

      //sum = 0; //opt
      sum = vdupq_n_f32(0); //we should reset sum here.
      IS_X = IMG_SIZE*x;

      for(y = 0; y < IMG_SIZE-DEGREE; y+=DEGREE) // each images
      {
        //sum += input[y] * weights[0][IS_X + y];
        Avec = vld1q_f32(&input[y]);
        Bvec = vld1q_f32(&weights[0][IS_X+y]);

        sum = vmlaq_f32(sum,Avec,Bvec);
      }

      for(;y<IMG_SIZE;y++)      //left cycle
        sum[0] +=  input[y] * weights[0][IS_X+y];

      sum[0] += sum[1]+sum[2]+sum[3];
      sum[0] += biases[0][x];
      hidden_layers[x] = sigmoid(sum[0]); //0~63 in hidden

      #pragma omp parallel for
      for(y=0;y<size;y+=4){         //cache tilling
          if(x==0){ 
              data[y+0]=biases[1][y+0];
              data[y+1]=biases[1][y+1];
              data[y+2]=biases[1][y+2];
              data[y+3]=biases[1][y+3];
          }
          data[y+0] += hidden_layers[x]*weights[1][x+size*(y+0)];
          data[y+1] += hidden_layers[x]*weights[1][x+size*(y+1)];
          data[y+2] += hidden_layers[x]*weights[1][x+size*(y+2)];
          data[y+3] += hidden_layers[x]*weights[1][x+size*(y+3)];
      }

    }
    for(x=0;x<size;x++){
        hidden_layers[size  + x] = sigmoid(data[x]);
    }

    clock_gettime(CLOCK_MONOTONIC,&forE);
    for1_s += (forE.tv_sec - forS.tv_sec) + 1e-9 * (forE.tv_nsec - forS.tv_nsec);

    //Between hidden layers
    /*--------------------------------------------------------*///this area will not be execute in small network
    clock_gettime(CLOCK_MONOTONIC,&forS);
    for(j = 2; j < depth; j++)
    {
      for(x = 0; x < size; x++)
      {
        sum =vdupq_n_f32(0);
        //sum = 0; //opt
        for(y = 0; y < size-DEGREE; y+=DEGREE)
        {
          //sum += hidden_layers[size * (j-1) + y] * weights[j][size * x + y];
          Avec = vld1q_f32(&hidden_layers[size*(j-1)+y]); //0~63, 64~127, 128 ~ 64*3-1
          Bvec = vld1q_f32(&weights[j][size*x+y]); // j,
          sum = vmlaq_f32(sum,Avec,Bvec);
        }
        for(;y<size;y++)      //left cycle
           sum[0] += hidden_layers[size*(j-1)+y]*weights[j][size*x+y];
        sum[0] += sum[1]+sum[2]+sum[3]; 
        sum[0] += biases[j][x];
        hidden_layers[size * j + x] = sigmoid(sum[0]);
      }
    }
    clock_gettime(CLOCK_MONOTONIC,&forE);
    for2_s += (forE.tv_sec - forS.tv_sec) + 1e-9 * (forE.tv_nsec - forS.tv_nsec);
    /*------------------------------------------------------*/
    
    // From the last hidden layer to the output layer
    clock_gettime(CLOCK_MONOTONIC,&forS);
    for(x = 0; x < DIGIT_COUNT; x++)
    {
      sum = vdupq_n_f32(0); //opt
      for(y = 0; y < size-DEGREE; y+=DEGREE)
      {
        //sum += hidden_layers[size * (depth-1) + y] * weights[depth][size * x + y];
        Avec = vld1q_f32(&hidden_layers[size*(depth-1)+y]);
        Bvec = vld1q_f32(&weights[depth][size*x+y]);
        sum = vmlaq_f32(sum,Avec,Bvec);
      }
      for(;y<size;y++)      //left cycle
         sum[0] += hidden_layers[size * (depth-1) + y] * weights[depth][size * x + y];

      sum[0]+= sum[1]+sum[2]+sum[3];
      sum[0] += biases[depth][x];
      output[x] = sigmoid(sum[0]);
    }
    clock_gettime(CLOCK_MONOTONIC,&forE);
    for3_s += (forE.tv_sec - forS.tv_sec) + 1e-9 * (forE.tv_nsec - forS.tv_nsec);

    // Find the answer
    max = 0; //opt
    label = 0; //opt

    for(x = 0; x < DIGIT_COUNT; x++)
    {
      if(output[x] > max)
      {
        label = x;
        max = output[x];
      }
    }

    // Store the result
    confidences[i] = max;
    labels[i] = label;
  }
  

  clock_gettime(CLOCK_MONOTONIC, &recE);
  sec2 += (recE.tv_sec - recS.tv_sec) + 1e-9 * (recE.tv_nsec - recS.tv_nsec);
  sec2_count ++;

}
