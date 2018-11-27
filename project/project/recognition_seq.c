#include <stdio.h>
#include <stdlib.h>
#include "recognition.h"
#include <math.h>

#define sigmoid(x) (1 / (1 + exp(-x)))

void recognition(float * images, float * network, int depth, int size, int * labels, float * confidences)
{
  int i, j, x, y;
  float *hidden_layers, *temp, **weights, **biases;

  hidden_layers = (float *)malloc(sizeof(float) * size * depth);
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

  // Recognize numbers
  for(i = 0; i < IMG_COUNT; i++)
  {
    float * input = images + IMG_SIZE * i;
    float output[DIGIT_COUNT];

    // From the input layer to the first hidden layer
    for(x = 0; x < size; x++)
    {
      float sum = 0;
      for(y = 0; y < IMG_SIZE; y++)
      {
        sum += input[y] * weights[0][IMG_SIZE * x + y];
      }
      sum += biases[0][x];
      hidden_layers[x] = sigmoid(sum);
    }

    // Between hidden layers
    for(j = 1; j < depth; j++)
    {
      for(x = 0; x < size; x++)
      {
        float sum = 0;
        for(y = 0; y < size; y++)
        {
          sum += hidden_layers[size * (j-1) + y] * weights[j][size * x + y];
        }
        sum += biases[j][x];
        hidden_layers[size * j + x] = sigmoid(sum);
      }
    }
    
    // From the last hidden layer to the output layer
    for(x = 0; x < DIGIT_COUNT; x++)
    {
      float sum = 0;
      for(y = 0; y < size; y++)
      {
        sum += hidden_layers[size * (depth-1) + y] * weights[depth][size * x + y];
      }
      sum += biases[depth][x];
      output[x] = sigmoid(sum);
    }

    // Find the answer
    float max = 0;
    int label = 0;
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
