#include "MicroFlow.h"

void setup(){
  Serial.begin(9600);
  int topology[] = {2, 2, 2, 1};
  double weights[] = {6.5388827, 2.3116155, 6.5393276, 2.311627, -2.8204367, -2.5849876, 3.4741454, -1.7074409, -2.5904362, -0.8814233};
  double biases[] = {-1.4674287, -3.13011, 0.36903697, -0.27291444, 1.5541532};
  double inputs[] = {0, 0};
  double output[1] = {};
  int layers = 4;
  
  MicroMLP mlp(layers, topology, weights, biases, SIGMOID);
  
  mlp.feedforward(inputs, output);
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);

  inputs[0] = 1;
  mlp.feedforward(inputs, output);
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);

  inputs[1] = 1;
  mlp.feedforward(inputs, output);
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);

  inputs[0] = 0;
  mlp.feedforward(inputs, output);
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);
}
void loop(){
  
}
