import numpy as np
from network import KohonenNetwork
import random


def printShape(shape: np.array):
    for i in range (0, 6):
         print(shape[0 + 6*i], shape[1 + 6*i], shape[2 + 6*i], shape[3 + 6*i], shape[4 + 6*i], shape[5 + 6*i])

def crashImage(image_matrix, noise_level):
    new_image_matrix = image_matrix.copy()

    random_indices = random.sample(range(0, new_image_matrix.size), int(noise_level * new_image_matrix.size / 10))

    for i in random_indices:
        new_image_matrix[i] = np.abs(new_image_matrix[i] - 1)

    return new_image_matrix

def printResult(result) :
    class_number = 1

    print(f'Output neurons values:')

    for prediction in result:
        print(f'y{class_number}: {prediction}')
        class_number += 1


def main():
    first_shape = np.array([
        0, 0, 1, 1, 0, 0,
        0, 1, 0, 0, 1, 0,
        1, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
    ])
    second_shape = np.array([
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 1,
        0, 1, 0, 0, 1, 0,
        0, 0, 1, 1, 0, 0,
    ])
    third_shape = np.array([
        1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1,
        1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1,
        1, 1, 1, 1, 1, 1,
    ])
    fourth_shape = np.array([
        0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1,
        1, 1, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1,
    ])
    fifth_shape = np.array([
        1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1,
        0, 0, 0, 0, 1, 1,
        1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 0,
    ])

    images = [first_shape, second_shape, third_shape, fourth_shape, fifth_shape]

    trueResults = np.array([
        [1, 0, 0, 0, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 1, 0, 0],
        [0, 0, 0, 1, 0],
        [0, 0, 0, 0, 1]])

    network = KohonenNetwork(36, 5, 0.05, 0.01)

    network.training(images)

    for j in range (0, len(images)):
        for i in range (0, 5) :
            print("***** " + str(i*10) + "% noised")
            crashedImage = crashImage(images[j], i)
            printShape(crashedImage)
            printResult(network.recognizeImage(crashedImage))
            print("\n")


if __name__ == '__main__':
    main()
