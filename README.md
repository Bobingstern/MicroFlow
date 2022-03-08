# MicroFlow
Export and run tensorflow models on arduino microcontrollers!

## Usage

### Installation
Currently, this library is not available on the arduino library manager so go ahead and download the latest release at: https://github.com/Bobingstern/MicroFlow/releases and at it to your sketch folder

### Training
Train the model using tensorflow on whatever system you like. You can refer to the training examples for sin and XOR [here](https://github.com/Bobingstern/MicroFlow/blob/main/exmaples/Arduino_MicroFlow.ipynb)
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

Great! Now that we have a fully trained tensorflow model, let's take a look at how to export it into MicroFlow.
The following script will take in a model as a parameter and save it's weights and biases into a format that MicroFlow can use, which is just a flat array of weights and biases. That file it saves this to can be specified via the ```filename``` parameter in the function. 

The output file will contain something along the lines of:

```
weights: {...}
biases: {...}
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

    print("Weights: ", z)
    print("Biases: ", b)

```
