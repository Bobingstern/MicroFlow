![My epic logo](https://github.com/Bobingstern/MicroFlow/blob/main/MicroFlow%20(1280%20%C3%97%20300%20px).png?raw=true)

# MicroFlow
Export and run TensorFlow models on Arduino microcontrollers!

### Why?
Currently, tflite-micro can only run on high memory boards like the Nano 33 BLE. I wanted to make it possible to run TensorFlow models on boards even with low amounts of memory and make the process as simple as possible while still making it useful and efficient. If eventually they do manage to make it possible to run tflite on low memory boards like the Uno then this library will become obsolete :( Also, I wanted to work on some linear algebra :) 

### Update Notes:
Version `1.1.3` released. I added a new example.

# Usage

### Installation
This library is available on the Arduino library manager, just look up "MicroFlow" and install the latest version. If you don't want to do that then go ahead and download the latest release at: https://github.com/Bobingstern/MicroFlow/releases and at it to your libraries folder

### Training
Train the model using tensorflow on whatever system you like. You can refer to the training examples for sin and XOR [here](https://github.com/Bobingstern/MicroFlow/blob/main/examples/Arduino_MicroFlow.ipynb)
I will be using the XOR model for demonstration purposes. Currently only Multi-Layer Perceptrons (MLPs) are supported (Dense layers in keras).

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
This is where the magic happens! Now that you have the weights, biases and architecture of the neural network, you are ready to run it on an Arduino board. **Be cautious of memory! If your neural network is too big you will run out of memory and not be able to run it!**

Here is the XOR example in Arduino code using the model we trained earlier:
```cpp
#include "MicroFlow.h"

void setup(){
  Serial.begin(9600);
  int topology[] = {2, 2, 2, 1};
  double weights[] = {6.5388827, 2.3116155, 6.5393276, 2.311627, -2.8204367, -2.5849876, 3.4741454, -1.7074409, -2.5904362, -0.8814233};
  double biases[] = {-1.4674287, -3.13011, 0.36903697, -0.27291444, 1.5541532};
  double inputs[] = {0, 0};
  double output[1] = {};
  int[] activations = {SIGMOID, SIGMOID, SIGMOID} //Activations for each layer excluding the input layer
  int layers = 4;
  
  MicroMLP mlp(layers, topology, weights, biases, SIGMOID); //Create the Multi-Layer Perceptron
  //MicroMLP mlp(layers, topology, weights, biases, activations); //This is also valid.

  mlp.feedforward(inputs, output); //Run a feedforward pass
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
```
#### MicroMLP
The ```MicroMLP``` constructor takes in 5 parameters. `layers` `topology` `weights` `biases` `activation` 

`layers` is the total number of layers in the neural network *(int)*

`topology` is the architecture of the neural network *(int array)*

`weights` is the weights obtained by training *(double array)*

`biases` is the biases obtained by training *(double array)*

`activation` is the activation of the network. Activations range from: **0** `SIGMOID`, **1** `TANH`, **2** `RELU`, **3**, `EXPONENTIAL` (e^x), **4**,`SWISH` (x * sigmoid(x)), **-1** `LINEAR` (no activation). This parameter can be an int array containing the activation for each layer (excluding the input). For example, a network with a topology `{2, 2, 2, 1}` could have an activation of `{TANH, TANH, TANH}` or just `TANH` *(int or int array)*

#### feedforward
The `feedforward` method takes in 2 parameters. `inputs` and `outputs`.

`inputs` are just the inputs for the feed forward pass *(double array)*

`outputs` is the array to store the output of the feed forward pass *(double array)*

### Larger Example
![Dino Game played by MicroFlow](https://raw.githubusercontent.com/Bobingstern/MicroFlow/main/assets/dino.gif)

Heres an example of MicroFlow playing the Chrome Dino game on an LCD. It was trained by saving the data of me playing and then training of off it. It plays at 100% accuracy now even with a relatively small neural network. Currently it's only been tested on the Arduino Mega, the Uno doesn't have enough memory. Check out the code in the examples folder https://github.com/Bobingstern/MicroFlow/tree/main/examples

### Don't have an Arduino right now?
Check out the repl I made to demonstrate the library without an Arduino board. It uses exactly the same code and just includes a few extra header files. Check it out [here](https://replit.com/@Bobingstern/MicroFlow-Demo?v=1)

## Compatibility

| Arduino          | Chip           | RAM   | XOR     | Sin     |
| ---------------- | -------------- | ----- | ------- | ------- |
| Arduino Uno      | ATmega328P     | 2KB   | :white_check_mark:     | :x:     |
| Arduino Mega (Recommended)     | ATmega2560     | 8KB   | :white_check_mark: | :white_check_mark: |
| Arduino Due      | SAM3X8E M3     | 96KB  | :white_check_mark: | :white_check_mark: |
| Adafruit PyGamer | ATSAMD51J19 M4 | 192KB | :white_check_mark: | :white_check_mark: |
| ESP32            | ESP-WROOM-32   | 320KB | :white_check_mark: | :white_check_mark: |

If you've tested a board that is not on this list, please make an issue and tell me!

# Current Issues
Currently, the Arduino Uno cannot run the Sin test. If I've done the math correctly, the Uno should be able to run the model with more than 400 bytes of memory left but it just kind of dies. I've tested it on the Mega while monitoring the memory and it does not exceed the limit of the Uno. I assume it is a problem with the Serial Monitor. I'll try to get this sorted out in the upcoming updates. If anyone knows what is going on, please do not hesitate to make a pull request or an issue. 

# Want to contribute?
This library in still in very early stages of development so bugs are to be expected. If you find one, please make an issue and provide steps on how to recreate it. If you want to test different boards, feel free to run the the [board test file](https://github.com/Bobingstern/MicroFlow/blob/main/examples/board_test/board_test.ino) on your board and make an issue with your test results. Thanks!

# Contributors
<a href="https://github.com/bobingstern/MicroFlow/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=bobingstern/MicroFlow" />
</a>
