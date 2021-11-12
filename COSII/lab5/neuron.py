class Neuron :

    def __init__(self, state=0):
        self.state = state

    def getState(self) :
        return self.state

    def setState(self, state: float) :
        self.state = state