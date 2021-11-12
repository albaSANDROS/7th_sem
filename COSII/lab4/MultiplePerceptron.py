import numpy as np
import random
import math
from Neuron import Neuron


# sigmoid
def activation_function(value):
    return 1 / (1 + np.exp(-value))


def randomInitThreshold(neurons):
    for index in range(0, len(neurons)):
        neurons[index].set_threshold(random.uniform(-1, 1))


def randomInitWeight(weights):
    for index, value in np.ndenumerate(weights):
        weights[index] = random.uniform(-1, 1)


class MultiplePerceptron:

    def __init__(self, input_neurons_amount, hide_neurons_amount,
                 output_neurons_amount, rate_learning, precision) -> None:
        self.input_neurons = [Neuron() for i in range(0, input_neurons_amount)]
        self.hide_neurons = [Neuron() for i in range(0, hide_neurons_amount)]
        self.output_neurons = [Neuron() for i in range(0, output_neurons_amount)]

        self.input_hide_weights = np.zeros((input_neurons_amount, hide_neurons_amount), dtype=float)
        self.hide_output_weights = np.zeros((hide_neurons_amount, output_neurons_amount), dtype=float)

        self.initNeuronsWeights()
        self.initNeuronsThreshold()

        self.rate_learning = rate_learning
        self.precision = precision

    def initNeuronsWeights(self):
        randomInitWeight(self.input_hide_weights)
        randomInitWeight(self.hide_output_weights)

    def initNeuronsThreshold(self):
        randomInitThreshold(self.hide_neurons)
        randomInitThreshold(self.output_neurons)

    def training_network(self, images, correct_outputs):
        trained_images = np.zeros(5, dtype=bool)

        while True:
            is_network_trained = True

            for i in range(0, len(images)):
                image = images[i].reshape(36)

                for j in range(0, len(self.input_neurons)):
                    self.input_neurons[j].set_state(image[j])

                self.calculate()

                if self.compareResults(correct_outputs[i]):
                    trained_images[i] = True
                    continue

                self.recalculateWeights(correct_outputs[i])

            for i in range(0, len(trained_images)):
                if not trained_images[i]:
                    is_network_trained = False

            if is_network_trained:
                return

    def calculate(self):
        self.calculateSecondLayerOutputs(self.input_neurons, self.hide_neurons, self.input_hide_weights)
        self.calculateSecondLayerOutputs(self.hide_neurons, self.output_neurons, self.hide_output_weights)

    def calculateSecondLayerOutputs(self, firstLayer, secondLayer, weights):
        for i in range(0, len(secondLayer)):
            result = 0

            for j in range(0, len(firstLayer)):
                result += firstLayer[j].get_state() * weights[j][i]

            secondLayer[i].set_state(activation_function(result + secondLayer[i].get_threshold()))

    def recalculateWeights(self, correct_outputs):
        self.recalculateHideOutputsWeights(correct_outputs)
        self.recalculateInputHideWeights(correct_outputs)

    def recalculateHideOutputsWeights(self, correct_outputs):
        for i in range(0, len(self.output_neurons)):
            threshold = self.output_neurons[i].get_threshold() + self.rate_learning * \
                        (correct_outputs[i] - self.output_neurons[i].get_state()) * \
                        self.output_neurons[i].get_state() * (1 - self.output_neurons[i].get_state())

            self.output_neurons[i].set_threshold(threshold)

        height, width = self.hide_output_weights.shape
        for i in range(0, height):
            for j in range(0, width):
                value = self.hide_output_weights[i][j] + self.rate_learning * \
                                                 (correct_outputs[j] - self.output_neurons[j].get_state()) * \
                                                 self.output_neurons[j].get_state() * (
                                                         1 - self.output_neurons[j].get_state()) * \
                                                 self.hide_neurons[i].get_state()
                self.hide_output_weights[i][j] = value

    def recalculateInputHideWeights(self, correct_outputs):
        height, width = self.input_hide_weights.shape
        for i in range(0, height):
            for j in range(0, width):
                e = 0
                for k in range(0, len(self.output_neurons)):
                    e += self.hide_output_weights[j][k] * (correct_outputs[k] - self.output_neurons[k].get_state()) * \
                         self.output_neurons[k].get_state() * (1 - self.output_neurons[k].get_state())

                self.input_hide_weights[i][j] = self.input_hide_weights[i][j] + self.rate_learning * \
                                                self.hide_neurons[j].get_state() * (
                                                        1 - self.hide_neurons[j].get_state()) * \
                                                e * self.input_neurons[i].get_state()

                if i == 0:
                    threshold = self.hide_neurons[j].get_threshold() + self.rate_learning * \
                                self.hide_neurons[j].get_state() * (
                                                        1 - self.hide_neurons[j].get_state()) * e
                    self.hide_neurons[j].set_threshold(threshold)


    def compareResults(self, correct_outputs):
        max_mistake = -1

        for i in range(0, len(self.output_neurons)):
            mistake = abs(correct_outputs[i] - self.output_neurons[i].get_state())

            if mistake > max_mistake:
                max_mistake = mistake

        return self.precision > max_mistake

    def recognizeImage(self, noise_image):
        height, width = noise_image.shape
        image = noise_image.reshape(height * width)
        for i in range(0, len(self.input_neurons)):
            self.input_neurons[i].set_state(image[i])

        self.calculate()

        resultImage = [self.output_neurons[i].get_state() for i in range(0, len(self.output_neurons))]

        return np.array(resultImage)


