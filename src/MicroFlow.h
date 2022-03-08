#ifndef MicroFlow_h
#define MicroFlow_h

#include "Arduino.h"


static const byte LOGISTIC = 0;
static const byte TANH = 1;
static const byte RELU = 2;
static const byte LINEAR = 3;
bool matrix_multiply(int R, int R2, int C2, double* a, double* b, double* result, int size);
void activate(int l, double* z, int activation);
void feedforward(int layers, int* topology, double* weights, double* biases, double* input, int activ, double* out);


#endif