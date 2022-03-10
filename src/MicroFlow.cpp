#include "Arduino.h"
#include "MicroFlow.h"


// bool matrix_multiply(int R, int R2, int C2, double* a, double* b, double* result, int size){
//  // double result[R2];
//   double cpy[R];
//   for (int i=0;i<R;i++){
//     cpy[i] = a[i];
//   }
//   int columnB = 0;
//   for (int i = 0; i < R2; i++) {
//     int bIter = columnB;
//     double sum = 0;
//     for (int j = 0; j < R; j++) {
//       //printf("==%f %f==", a[j], b[bIter]);
//       sum += cpy[j] * b[bIter];
//       bIter += R2;
//     }
//     //printf("\n");
//     result[i] = sum;
//     if (i < size) {
//     } else {
//       return false;
//     }
//     columnB ++;
//   }
//   return true;
//   // return result;
// }

MicroMLP::MicroMLP(int la, int* top, double* w, double* b, int* a){
  layers = la;
  topology = top;
  weights = w;
  biases = b;
  activations = a;
}

MicroMLP::MicroMLP(int la, int* top, double* w, double* b, int a){
  layers = la;
  topology = top;
  weights = w;
  biases = b;
  sameActiv = a;
  allSameActiv = true;
}

void activate(int l, double* z, int activation) {
  for (int i = 0; i < l; i++) {
    if (activation == SIGMOID) {
      z[i] = 1 / (1 + exp(-z[i]));
    } else if (activation == TANH) {
      z[i] = tanh(z[i]);
    } else if (activation == EXPONENTIAL){
      z[i] = exp(z[i]);
    } else if (activation == SWISH){
      z[i] = z[i] / (1 + exp(-z[i]));
    } else if (activation == RELU){
      z[i] = fmax(0, z[i]);
    }
  }
}

void MicroMLP::feedforward(double* input, double* out){
  int maxLayer = 0;
  for (int i=0;i<layers;i++){
    if (topology[i] > maxLayer){
      maxLayer = topology[i];
    }
  }

  double x[maxLayer];
  for (int i=0;i<topology[0];i++){
    x[i] = input[i];
  }
  int weightAdder = 0;
  int biasAdder = 0;
  for (int l=0;l<layers-1;l++){

    //Matrix----
    double cpy[topology[l]];
    for (int i=0;i<topology[l];i++){
      cpy[i] = x[i];
    }
    int columnB = 0;
    for (int i = 0; i < topology[l+1]; i++) {
      int bi = columnB;
      double sum = 0;
      
      for (int j = 0; j < topology[l]; j++) {
        sum += cpy[j] * weights[bi+weightAdder];
        bi += topology[l+1];
      }
      x[i] = sum;
      columnB ++;
    }
    //matrix_multiply(topology[l], topology[l+1], topology[l], x, weightStack, x, topology[l+1]);
    
    for (int i=0;i<topology[l+1];i++){
      x[i] += biases[i+biasAdder];
    }
    if (l != layers-2){
      if (!allSameActiv)
        activate(topology[l+1], x, activations[l]);
      else
        activate(topology[l+1], x, sameActiv);
    }
    weightAdder += topology[l]*topology[l+1];
    biasAdder += topology[l+1];
  }
  for (int i=0;i<topology[layers-1];i++){
    out[i] = x[i];
  }
}