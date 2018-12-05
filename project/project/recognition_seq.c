#include <stdio.h>
#include <stdlib.h>
#include "recognition.h"
#include <math.h>

#define sigmoid(x) (1 / (1 + exp(-x)))

void recognition(float * images, float * network, int depth, int size, int * labels, float * confidences)
{
  clock_gettime(CLOCK_MONOTONIC, &recS);

  int i, j, x, y;
  //float *hidden_layers, *temp, **weights, **biases;
  float *hidden_layers, **weights, **biases; // Delete unused variable *temp

  // pre- calculated
  int sizedepth = size==64 ? depth<<6 : size * depth;
  int sizeIMG_SIZE = size==64 ? IMG_SIZE<<6 : size * IMG_SIZE;
  int sizesize = size == 64 ? 1<<12 : size * size;
  int DIGIT_COUNTsize = size == 64 ? DIGIT_COUNT << 6 : DIGIT_COUNT * size;

  hidden_layers = (float *)malloc((sizedepth)<<2); //sizeof(float) = 4
  //hidden_layers = (float *)malloc((size*depth)<<2); //sizeof(float) = 4
  weights = (float **)malloc((depth + 1)<<2); //sizeof(float*) = 4
  biases = (float **)malloc((depth + 1)<<2); //sizeof(float *) = 4

  // Set pointers for weights and biases
  // 1. Input layer
  weights[0] = network;
  biases[0] = weights[0] + sizeIMG_SIZE;
  // 2. Hidden layers
  for(i = 1; i < depth; i++) //depth가 1이면 loop unroll no need
  {
    weights[i] = network + (sizeIMG_SIZE + size) + (sizesize + size) * (i-1);
    biases[i] = weights[i] + sizesize;
  }
  // 3. Output layer
  weights[depth] = weights[depth - 1] + sizesize + size;
  biases[depth] = weights[depth] + DIGIT_COUNTsize;


  // variables in for
  float * input;
  float32x4_t Avec, Bvec;

  float output[DIGIT_COUNT];
  float32x4_t sum;

  int cmVar1 = 0; //variable for code motion
  int cmVar2 = 0; //variable for code motion

  float max = 0;
  int label = 0;
  
  // Recognize numbers
  for(i = 0; i < IMG_COUNT; ++i)
  { 
    //input = images + IMG_SIZE * i;
    (input) = (images + IMG_SIZE * i);

    // From the input layer to the first hidden layer
    for(x = 0; x < size; ++x)
    {
      sum = vdupq_n_f32(0); //we should reset sum here.
      cmVar1 = IMG_SIZE * x;
      for(y = 0; y < IMG_SIZE-1; y+=4)
      {
        //sum += input[y] * weights[0][cmVar1 + y];
        //sum += input[y+1] * weights[0][cmVar1 + y + 1];
        //sum += input[y+2] * weights[0][cmVar1 + y + 2];
        //sum += input[y+3] * weights[0][cmVar1 + y + 3];
        
        Avec = vld1q_f32(&input[y+0]);
        Bvec = vld1q_f32(&weights[0][cmVar1+y+0]);

        sum = vmlaq_f32(sum, Avec, Bvec );
      }

      for(;y<IMG_SIZE;++y)
          sum[0] += input[y] * weights[0][cmVar1+y];
      sum[0] += sum[1]+sum[2]+sum[3];
      sum[0] += biases[0][x];
      hidden_layers[x] = sigmoid(sum[0]);
    }

    // Between hidden layers
    for(j = 1; j < depth; ++j)
    {

      cmVar1 = size == 64 ? (j-1) << 6 : size * (j-1);
      for(x = 0; x < size; ++x)
      {
       sum = vdupq_n_f32(0); //we should reset sum here.
       cmVar2 = size == 64 ? x << 6 : size * x;
        for(y = 0; y < size-1; y+=4)
        {
        //  sum += hidden_layers[cmVar1 + y] * weights[j][cmVar2 + y];
        //  sum += hidden_layers[cmVar1 + y + 1] * weights[j][cmVar2 + y + 1];
        //  sum += hidden_layers[cmVar1 + y + 2] * weights[j][cmVar2 + y + 2];
        //  sum += hidden_layers[cmVar1 + y + 3] * weights[j][cmVar2 + y + 3];
                  
          Avec = vld1q_f32(&hidden_layers[cmVar1 + y]);
          Bvec = vld1q_f32(&weights[j][cmVar2+y]);

	  sum = vmlaq_f32(sum, Avec, Bvec );         
        }
        for(;y<size;++y)
            sum[0] += hidden_layers[cmVar1+y] * weights[j][cmVar2 + y];
	sum[0] += sum[1]+sum[2]+sum[3];
        sum[0] += biases[j][x];
        hidden_layers[cmVar1 + size + x] = sigmoid(sum[0]);
      }
    }
    
    // From the last hidden layer to the output layer 
    for(x = 0; x < DIGIT_COUNT; ++x)
    {
      sum = vdupq_n_f32(0); //we should reset sum here.
      cmVar1 = size==64 ? x << 6 : size * x;
      for(y = 0; y < size-1; y+=4)
      {
        //sum += hidden_layers[sizedepth - size + y] * weights[depth][cmVar1 + y];
        //sum += hidden_layers[sizedepth - size + y + 1] * weights[depth][cmVar1 + y + 1];
       // sum += hidden_layers[sizedepth - size + y + 2] * weights[depth][cmVar1 + y + 2];
       // sum += hidden_layers[sizedepth - size + y + 3] * weights[depth][cmVar1 + y + 3];

        Avec = vld1q_f32(&hidden_layers[sizedepth - size + y]);
        Bvec = vld1q_f32(&weights[depth][cmVar1+y]);
        
	sum = vmlaq_f32(sum, Avec, Bvec ); 
      }
      for(;y<size;++y)
          sum[0] += hidden_layers[sizedepth - size + y]*weights[depth][cmVar1+y];
	sum[0] += sum[1]+sum[2]+sum[3];

      sum[0] += biases[depth][x];
      output[x] = sigmoid(sum[0]);
    }

    // Find the answer
    max = 0;
    label = 0;
    for(x = 0; x < DIGIT_COUNT; ++x)
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
