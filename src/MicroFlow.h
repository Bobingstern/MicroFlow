#ifndef MicroFlow_h
#define MicroFlow_h

#include "Arduino.h"


static const int SIGMOID = 0;
static const int TANH = 1;
static const int RELU = 2;
static const int EXPONENTIAL = 3;
static const int SWISH = 4;
static const int LINEAR = -1;


void activate(int l, double* z, int activation);

class MicroMLP{
  public:
    double* weights;
    double* biases;
    int layers;
    int* topology;
    int* activations;
    int sameActiv;
    bool allSameActiv = false;
    MicroMLP(int la, int* top, double* w, double* b, int* a);
    MicroMLP(int la, int* top, double* w, double* b, int a);
    //bool matrix_multiply(int R, int R2, int C2, double* a, double* b, double* result, int size);
    void feedforward(double* input, double* out);

};


#endif