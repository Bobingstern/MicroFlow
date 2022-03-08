# MicroFlow
Export and run TensorFlow models on Arduino microcontrollers!

## Update Notes:
Version `1.0.0-alpha` is finally finished! Eventually I will make this library class based rather than C headers and methods so make sure to stay updated for that. Also, a floated version of this library will be released soon that used floats rather than doubles to save a bit of memory.

## Usage

### Installation
Currently, this library is not available on the Arduino library manager so go ahead and download the latest release at: https://github.com/Bobingstern/MicroFlow/releases and at it to your sketch folder

### Training
Train the model using tensorflow on whatever system you like. You can refer to the training examples for sin and XOR [here](https://github.com/Bobingstern/MicroFlow/blob/main/examples/Arduino_MicroFlow.ipynb)
I will be using the XOR model for demonstration purposes

```py
# TensorFlow is an open source machine learning library
import tensorflow as tf
# Keras is TensorFlow's high-level API for deep learning
from tensorflow import keras
# Numpy is a math library
import numpy as np

#Create a model with a (2, 2, 2, 1) architecture
MODEL = keras.Sequential()
MODEL.add(keras.layers.Dense(2, activation='sigmoid', input_shape=(2,)))
MODEL.add(keras.layers.Dense(2, activation='sigmoid'))
# Final layer is a single neuron, since we want to output a single value
MODEL.add(keras.layers.Dense(1))

# Compile the model using the standard 'adam' optimizer and the mean squared error or 'mse' loss function for regression.
MODEL.compile(optimizer=keras.optimizers.Adam(learning_rate=1e-2), loss='mse')

#Train the model
training_data = np.array([[0, 0],[1, 0],[0, 1],[1, 1]], "float32")

# the four expected results in the same order
target_data = np.array([[0],[1],[1],[0]], "float32")

MODEL.fit(training_data, target_data, epochs=1000)
```

Great! Now that we have a fully trained TensorFlow model, let's take a look at how to export it into MicroFlow.
The following script will take in a model as a parameter and save it's weights and biases into a format that MicroFlow can use, which is just a flat array of weights and biases. The name of the file it saves this to can be specified via the ```filename``` parameter in the function. The ```Architecture``` functionality is still in alpha so please contact me if there is an issue

The output file will contain something along the lines of:

```
weights: {...}
biases: {...}
Architecture: {...}
```

```py
def weights_to_cpp(model, filename="weights_and_biases.txt"):
    model.summary()
    weights = []
    biases = []
    for l in range(len(model.layers)):
        W, B = model.layers[l].get_weights()
        weights.append(W.flatten())
        biases.append(B.flatten())
    
    z = []
    b = []
    for i in np.array(weights):
        for l in i:
            z.append(l)
    for i in np.array(biases):
        for l in i:
            b.append(l)
    with open(filename, "w") as f:
      f.write("weights: {")
      for i in range(len(z)):
        if (i < len(z)-1):
          f.write(str(z[i])+", ")
        else:
          f.write(str(z[i]))
      f.write("}\n\n")

      f.write("biases: {")
      for i in range(len(b)):
        if (i < len(b)-1):
          f.write(str(b[i])+", ")
        else:
          f.write(str(b[i]))
      f.write("}\n\n")
    
      arch = []
    
      arch.append(model.layers[0].input_shape[1])
      for i in range(1, len(model.layers)):
          arch.append(model.layers[i].input_shape[1])
      arch.append(model.layers[len(model.layers)-1].output_shape[1])
      f.write("Architecture: {")
      for i in range(len(arch)):
          if (i < len(arch)-1):
              f.write(str(arch[i])+", ")
          else:
              f.write(str(arch[i]))
      f.write("}")
      print("Architecture (alpha):", arch)
      print("Layers: ", len(arch))
    print("Weights: ", z)
    print("Biases: ", b)

```
### Running on Arduino
This is where the magic happens! Now that you have the weights, biases and architecture of the neural network, you are ready to run it on an Arduino board. **Be cautious of memory! If you neural network is too big you will run out of memory and not be able to run it!**

Here is the XOR example in Arduino code using the model we trained earlier:
```c++
#include "MicroFlow.h"

void setup(){
  Serial.begin(9600);
  //Architecture of the network
  int topology[] = {2, 2, 2, 1};
  //Total number of layers
  int layers = 4;
  //Weights and biases obtained from training
  double weights[] = {6.5388827, 2.3116155, 6.5393276, 2.311627, -2.8204367, -2.5849876, 3.4741454, -1.7074409, -2.5904362, -0.8814233};
  double biases[] = {-1.4674287, -3.13011, 0.36903697, -0.27291444, 1.5541532};
  //Inputs and outputs for testing the network
  double inputs[] = {0, 0};
  double output[1] = {};
  
  
  feedforward(layers, topology, weights, biases, inputs, LOGISTIC, output); //Feedforward pass
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);

  inputs[0] = 1;
  feedforward(layers, topology, weights, biases, inputs, LOGISTIC, output);
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);

  inputs[1] = 1;
  feedforward(layers, topology, weights, biases, inputs, LOGISTIC, output);
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);

  inputs[0] = 0;
  feedforward(layers, topology, weights, biases, inputs, LOGISTIC, output);
  Serial.print("Inputs: ");Serial.print(inputs[0]);Serial.print(", ");Serial.println(inputs[1]);
  Serial.print("Neural Network Output: ");Serial.println(output[0]);
}
void loop(){
  
}
```
The ```feedforward``` method takes in 6 parameters. `layers` `topology` `weights` `biases` `inputs` `activation` `output`

`layers` is the total number of layers in the neural network (int)

`topology` is the architecture of the neural network (int array)

`weights` is the weights obtained by training (double array)

`biases` is the biases obtained by training (double array)

`inputs` is the input for the feed forward pass (double array)

`activation` is the activation of the network. Currently, it is the same activation for each layer. Activation range from: **0** `LOGISTIC` (sigmoid), **1** `TANH`, **2** `RELU`, **3**, `LINEAR` (none). (int)

`output` where you want to output of the feed forward pass to be stored. (double array)

### Don't have an Arduino right now?
Check out the repl I made to demonstrate the library without an Arduino board. It uses exactly the same code and just includes a few extra header files. Check it out [here](https://replit.com/@Bobingstern/MicroFlow-Testing?v=1)

## Compatibility

| Arduino      | XOR     | Sin      |
| ------------ | ------- | -------- |
| Arduino Uno  | :heavy_minus_sign: | :heavy_minus_sign:  |
| Arduino Mega | :white_check_mark: | :white_check_mark:  |
| Arduino Due  | :heavy_minus_sign: | :heavy_minus_sign:  |

If you've tested a board that is not on this list, please make an issue and tell me!

# Found a Bug?
This library in still in very early stages of development so bugs are to be expected. If you find one, please make an issue and provide steps on how to recreate it. Thanks!

# Contributors
<a href="https://github.com/Bobingstern/MicroFlow/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Bobingstern/MicroFlow" />
</a>

