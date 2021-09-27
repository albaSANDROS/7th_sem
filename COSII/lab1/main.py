from PIL import Image
from numpy import asarray
import seaborn as sn
import cv2
import numpy as np
from matplotlib import pyplot as plt


def to_one_arr(source):
    output = []
    for i in range(255):
        output.append(0)

    for x in range(len(source)):
        for y in range(len(source[0])):
            var = source[x][y][0] * 0.3 + source[x][y][1] * 0.58 + source[x][y][2] * 0.12
            var = round(var)
            if var > 255:
                var = 255
            output[var] = output[var] + 1
    return output


def element_v(source):
    f = 230
    g = 100

    output = np.copy(source)
    for x in range(len(source)):
        for y in range(len(source[0])):
            for z in range(len(source[0][0])):
                pixel = output[x][y][z]
                if pixel < f:
                    pixel = 0
                else:
                    pixel = (g / (255 - f)) * (pixel - f)
                output[x][y][z] = pixel
    return output


def element_g(source):
    f = 110
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


def previt(source):
    output = np.copy(source)
    for x in range(1, len(source) - 1):
        for y in range(1, len(source[0]) - 1):
            for z in range(len(source[0][0])):
                h1 = - int(source[x - 1][y - 1][z]) - source[x - 1][y][z] - source[x - 1][y + 1][z] + source[x + 1][y - 1][z] + source[x + 1][y][z] + source[x + 1][y + 1][z]
                h2 = - int(source[x - 1][y - 1][z]) - source[x][y - 1][z] - source[x + 1][y - 1][z] + source[x - 1][y + 1][z] - source[x][y + 1][z] - source[x + 1][y + 1][z]
                h = max(h1, h2)
                if h < 0:
                    h = 0
                elif h > 255:
                    h = 255
                output[x][y][z] = h
    return output


if __name__ == '__main__':
    image = Image.open("img.jpg")

    plt.plot(to_one_arr(asarray(image)))
    plt.show()

    array = asarray(image)
    v = element_v(array)
    g = element_g(array)
    pr = previt(array)

    plt.plot(to_one_arr(v))
    plt.show()
    plt.plot(to_one_arr(g))
    plt.show()
    plt.plot(to_one_arr(pr))
    plt.show()

    Image.fromarray(array).save("default.jpg")

    Image.fromarray(pr).save("previt.jpg")
    Image.fromarray(v).save("v.jpg")
    Image.fromarray(g).save("g.jpg")
    Image.fromarray(element_g(asarray(Image.open("bad.jpg")))).save("good.jpg")




@staticmethod
    def roberts_operator(image):
        im = image.copy()
        pixels = im.load()
        w, h = im.size

        for i in range(w - 1):
            for j in range(h - 1):
                pixels[i, j] = tuple(
                    map(lambda x1, x2, x3, x4: int(math.sqrt((x1 - x2) ** 2 + (x2 - x3) ** 2)), pixels[i + 1, j],
                        pixels[i, j + 1], pixels[i, j], pixels[i + 1, j + 1]))

        return im



    @staticmethod
    def logarithmic_correction(image):
        constant = input('Enter constant:')
        im = image.copy()
        pixels = im.load()

        for i in range(im.size[0]):
            for j in range(im.size[1]):
                pixels[i, j] = tuple(map(lambda x: int(constant * math.log(1 + x)), pixels[i, j]))
        return im
