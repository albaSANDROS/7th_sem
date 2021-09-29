from PIL import Image
from django.db.models.functions import math
from numpy import asarray
import cv2
import numpy as np
import math
from matplotlib import pyplot as plt


def to_one_arr(source):
    output = []
    for i in range(256):
        output.append(0)

    for x in range(len(source)):
        for y in range(len(source[0])):
            var = source[x][y][0] * 0.3 + source[x][y][1] * 0.58 + source[x][y][2] * 0.12
            var = round(var)
            if var > 255:
                var = 255
            output[var] = output[var] + 1
    return output


def roberts(source):
    output = np.copy(source)
    for x in range(1, len(source) - 1):
        for y in range(1, len(source[0]) - 1):
            for z in range(len(source[0][0])):
                h1 = int(source[x + 1][y][z]) - source[x][y + 1][z]
                h2 = int(source[x][y][z]) - source[x + 1][y + 1][z]
                h = math.sqrt((math.pow(h1, 2) + math.pow(h2, 2)))
                if h < 0:
                    h = 0
                elif h > 255:
                    h = 255
                output[x][y][z] = h
    return output

def bright(source):
    f = 180
    g = 20

    output = np.copy(source)
    for x in range(len(source)):
        for y in range(len(source[0])):
            for z in range(len(source[0][0])):
                pixel = output[x][y][z]
                if pixel > f:
                    pixel = 255
                else:
                    pixel = ((255 - g) / f) * pixel + g
                output[x][y][z] = pixel
    return output


def corr(image2):
    c = int(input('Enter constant:'))
    log_image = c * (np.log(image2 + 1))
    log_image = np.array(log_image, dtype=np.uint8)
    return log_image


if __name__ == '__main__':
    image = Image.open("img.jpg")
    imagecv = cv2.imread('img.jpg')

    plt.plot(to_one_arr(asarray(image)))
    plt.show()

    array = asarray(image)
    rf = roberts(array)
    correction = corr(imagecv)

    plt.plot(to_one_arr(rf))
    plt.show()
    plt.plot(to_one_arr(correction))
    plt.show()

    Image.fromarray(array).save("default.jpg")
    Image.fromarray(rf).save("roberts.jpg")
    Image.fromarray(correction).save("corr.jpg")
    Image.fromarray(bright(asarray(Image.open("bad.jpg")))).save("good.jpg")
