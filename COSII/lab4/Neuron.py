class Neuron:

    def __init__(self) -> None:
        self.threshold = None
        self.state = None

    def get_threshold(self):
       return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold

    def get_state(self):
        return self.state

    def set_state(self, state):
        self.state = state