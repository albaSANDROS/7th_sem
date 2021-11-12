import numpy as np
import random
from copy import deepcopy
from MultiplePerceptron import MultiplePerceptron
import Symbols


def noise_image(image, noise_percent):
    def revertValue(pixel):
        if pixel == 1:
            return 0
        return 1

    height, width = image.shape
    result_image = deepcopy(image)

    indexes = [index for index, value in np.ndenumerate(image)]
    pixels_needed_to_noise = int(noise_percent * height * width / 100)

    for i in range(0, pixels_needed_to_noise):
        index = random.choice(indexes)
        indexes.remove(index)
        result_image[index] = revertValue(result_image[index])

    return result_image


def print_image(image, title):
    height, width = image.shape

    print(title)

    for i in range(0, height):
        for j in range(0, width):
            if j % 5 == 0 and j != 0:
                print(str(image[i][j]))
            else:
                print(str(image[i][j]) + ' ', end='')


def print_similarity(similarity_result):
    for i in range(0, len(similarity_result)):
        print('\nSimilarity with ' + str(i + 1) + ' class is ' + str(similarity_result[i]), end='')


def calculate_similarity(noised_image, symbols):
    height, width = noised_image.shape
    result = np.zeros(5)

    for i in range(0, len(symbols)):
        counter = 0
        for j in range(0, height):
            for k in range(0, width):
                if symbols[i][j][k] == noised_image[j][k]:
                    counter += 1

        result[i] = counter / 36 * 100

    return result


if __name__ == '__main__':
    undef_image = np.array(Symbols.UNDEF)
    xor_image = np.array(Symbols.XOR)
    mult_image = np.array(Symbols.MULTIPLY)
    divide_image = np.array(Symbols.DIVIDE)
    plus_minus_image = np.array(Symbols.PLUS_MINUS)

    training_images = [undef_image, xor_image, mult_image, divide_image, plus_minus_image]
    training_results = [Symbols.UNDEF_TRUE_RESULT, Symbols.XOR_TRUE_RESULT, Symbols.MULTIPLY_TRUE_RESULT,
                        Symbols.DIVIDE_TRUE_RESULT, Symbols.PLUS_MINUS_TRUE_RESULT]

    perceptron = MultiplePerceptron(36, 10, 5, 0.2, 0.1)
    perceptron.training_network(training_images, training_results)

    ############## UNDEF ##############
    print('UNDEF')
    for noise_percent in range(20, 100, 30):
        noised_image = noise_image(undef_image, noise_percent)
        print_image(noised_image, '\nUNDEF with ' + str(noise_percent) + ' noise')

        result = perceptron.recognizeImage(noised_image)

        print('Result ', end='')
        for i in range(0, len(result)):
            print(str(result[i]) + ' ', end='')

        similarity_result = calculate_similarity(noised_image, training_images)
        print_similarity(similarity_result)

    ############## XOR ##############
    print('\n\nXOR')
    for noise_percent in range(20, 100, 30):
        noised_image = noise_image(xor_image, noise_percent)
        print_image(noised_image, '\nXOR with ' + str(noise_percent) + ' noise')

        result = perceptron.recognizeImage(noised_image)

        print('Result ', end='')
        for i in range(0, len(result)):
            print(str(result[i]) + ' ', end='')

        similarity_result = calculate_similarity(noised_image, training_images)
        print_similarity(similarity_result)


    ############## MULTIPLY ##############
    print('\n\nMULTIPLY')
    for noise_percent in range(20, 100, 30):
        noised_image = noise_image(mult_image, noise_percent)
        print_image(noised_image, '\nMULTIPLY with ' + str(noise_percent) + ' noise')

        result = perceptron.recognizeImage(noised_image)

        print('Result ', end='')
        for i in range(0, len(result)):
            print(str(result[i]) + ' ', end='')

        similarity_result = calculate_similarity(noised_image, training_images)
        print_similarity(similarity_result)


    ############## DIVIDE ##############
    print('\n\nDIVIDE')
    for noise_percent in range(20, 100, 30):
        noised_image = noise_image(divide_image, noise_percent)
        print_image(noised_image, '\nDIVIDE with ' + str(noise_percent) + ' noise')

        result = perceptron.recognizeImage(noised_image)

        print('Result ', end='')
        for i in range(0, len(result)):
            print(str(result[i]) + ' ', end='')

        similarity_result = calculate_similarity(noised_image, training_images)
        print_similarity(similarity_result)


    ############## PLUS_MINUS ##############
    print('\n\nPLUS_MINUS')
    for noise_percent in range(20, 100, 30):
        noised_image = noise_image(plus_minus_image, noise_percent)
        print_image(noised_image, '\nPLUS_MINUS with ' + str(noise_percent) + ' noise')

        result = perceptron.recognizeImage(noised_image)

        print('Result ', end='')
        for i in range(0, len(result)):
            print(str(result[i]) + ' ', end='')

        similarity_result = calculate_similarity(noised_image, training_images)
        print_similarity(similarity_result)





