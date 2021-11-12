from neuron import Neuron
import numpy as np

class KohonenNetwork :

     def __init__(self, inputLayerSize: int, outputLayerSize: int, beta: float, precision: float):
        self.inputLayer = [Neuron() for i in range(inputLayerSize)]
        self.outputLayer = [Neuron() for i in range(outputLayerSize)]
        self.initArrays()
        self.inputOutputLayersWeights = [[0 for i in range (outputLayerSize)] for j in range (inputLayerSize)]
        self.beta = beta
        self.precision = precision

        self.initializeWeights()
    

     def recognizeImage(self, image) :
        result = [0 for i in range (len(self.outputLayer))]

        for i in range (len(self.inputLayer)) :
            self.inputLayer[i].setState(image[i])

        self.calculateNeuronsOutputs()

        for i in range (len(self.outputLayer)) :
            result[i] = self.outputLayer[i].getState()

        return result
    

     def training(self, image) :
        trainedImages = [False for i in range (len(image))]
        winningCount = [0 for i in range (len(self.outputLayer))]

        isNotRecognizedImagesStayed = True

        while (isNotRecognizedImagesStayed) :
            isNotRecognizedImagesStayed = False

            for i in range (len(image)) :
                if (trainedImages[i]) :
                    continue

                for j in range (len(self.inputLayer)) :
                    self.inputLayer[j].setState(image[i][j])
                
                winnerIndex = self.findWinnerNeuron(winningCount)

                if (self.precision > self.findDistance(winnerIndex)) :
                    trainedImages[i] = True
                    continue

                self.correctWinnerNeuronWeights(winnerIndex)
            

            for trainedImage in trainedImages :
                if not trainedImage :
                    isNotRecognizedImagesStayed = True
                    break
                

     def initArrays(self) :
        for i in range (len(self.inputLayer)) :
            self.inputLayer[i] = Neuron()

        for i in range (len(self.outputLayer)) :
            self.outputLayer[i] = Neuron()

     def initializeWeights(self) :
        for i in range (len(self.inputOutputLayersWeights)) :
            for j in range (len(self.inputOutputLayersWeights[i])) :
                self.inputOutputLayersWeights[i][j] = np.random.random_sample()
    

     def findDistance(self, winnerNeuronIndex) :
        distance = 0
        for j in range (len(self.inputLayer)) :
            distance += pow(self.inputLayer[j].getState() - self.inputOutputLayersWeights[j][winnerNeuronIndex], 2)

        print(str(pow(distance, 2)))
        return pow(distance, 2)
    

     def findWinnerNeuron(self, winningCount) :
        winnerNeuronIndex = 0
        winnerValue = -1

        for i in range (len(self.outputLayer)) :
            distance = 0
            for j in range (len(self.inputLayer)) :
                distance += pow(self.inputLayer[j].getState() - self.inputOutputLayersWeights[j][i], 2)
            
            distance = pow(distance, 2)
            temp = abs(distance) * winningCount[i]

            if (winnerValue > temp or i == 0) :
                winnerValue = temp
                winnerNeuronIndex = i

        winningCount[winnerNeuronIndex] = winningCount[winnerNeuronIndex] + 1

        return winnerNeuronIndex
    

     def correctWinnerNeuronWeights(self, neuronIndex) :
        for i in range (len(self.inputOutputLayersWeights)) :
            weightValue = self.inputOutputLayersWeights[i][neuronIndex]
            self.inputOutputLayersWeights[i][neuronIndex] = weightValue + self.beta * (self.inputLayer[i].getState() - weightValue)


     def calculateNeuronsOutputs(self) :
        for i in range (len(self.outputLayer)) :
            inputWeightSum = 0
            for j in range (len(self.inputLayer)) :
                inputWeightSum += self.inputLayer[j].getState() * self.inputOutputLayersWeights[j][i]
            
            self.outputLayer[i].setState(inputWeightSum)
