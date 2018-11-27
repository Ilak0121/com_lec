#include <stdio.h>
#include <stdlib.h>
#include "recognition.h"
#include <math.h>

#define sigmoid(x) (1 / (1 + exp(-x)))

void recognition(float * images, float * network, int depth, int size, int * labels, float * confidences)
{
  int i, j, x, y;
  float *hidden_layers, *temp, **weights, **biases;

  // pre- calculated
  int sizedepth = size * depth;
  int sizedepth1 = size * (depth-1); 
  int sizeIMG_SIZE = size * IMG_SIZE;
  int sizesize = size * size;
  int DIGIT_COUNTsize = DIGIT_COUNT * size;

  hidden_layers = (float *)malloc((sizedepth)<<2); //sizeof(float) = 4
  //hidden_layers = (float *)malloc((size*depth)<<2); //sizeof(float) = 4
  weights = (float **)malloc((depth + 1)<<2); //sizeof(float*) = 4
  biases = (float **)malloc((depth + 1)<<2); //sizeof(float *) = 4

  // Set pointers for weights and biases
  // 1. Input layer
  weights[0] = network;
  biases[0] = weights[0] + sizeIMG_SIZE;
  // 2. Hidden layers
  for(i = 1; i < depth; i++)
  {
    weights[i] = network + (sizeIMG_SIZE + size) + (sizesize + size) * (i-1);
    biases[i] = weights[i] + sizesize;
  }
  // 3. Output layer
  weights[depth] = weights[depth - 1] + sizesize + size;
  biases[depth] = weights[depth] + DIGIT_COUNTsize;


  // variables in for
  float * input;
  float output[DIGIT_COUNT];
  float sum = 0;

  int cmVar1 = 0; //variable for code motion
  int cmVar2 = 0; //variable for code motion

  float max = 0;
  int label = 0;
  
  // Recognize numbers
  for(i = 0; i < IMG_COUNT; i++)
  {
    input = images + IMG_SIZE * i;

    // From the input layer to the first hidden layer
    for(x = 0; x < size; x++)
    {
  //    float sum = 0;
  cmVar1 = IMG_SIZE * x;
      for(y = 0; y < IMG_SIZE; y++)
      {
        sum += input[y] * weights[0][cmVar1 + y];
      }
      sum += biases[0][x];
      hidden_layers[x] = sigmoid(sum);
    }

    // Between hidden layers
    for(j = 1; j < depth; j++)
    {

      cmVar1 = size * (j-1);
      for(x = 0; x < size; x++)
      {
    //    float sum = 0;
       cmVar2 = size * x;
        for(y = 0; y < size; y++)
        {
          sum += hidden_layers[cmVar1 + y] * weights[j][cmVar2 + y];
        }
        sum += biases[j][x];
        hidden_layers[cmVar1 + size + x] = sigmoid(sum);
      }
    }
    
    // From the last hidden layer to the output layer 
    for(x = 0; x < DIGIT_COUNT; x++)
    {
      cmVar1 = size * x;
      //float sum = 0;
      for(y = 0; y < size; y++)
      {
        //sum += hidden_layers[size * (depth-1) + y] * weights[depth][cmVar1 + y];
        sum += hidden_layers[sizedepth1 + y] * weights[depth][cmVar1 + y];
      }
      sum += biases[depth][x];
      output[x] = sigmoid(sum);
    }

    // Find the answer
  //  float max = 0;
   // int label = 0;
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
}
