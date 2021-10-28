import numpy as np
import sys
import random

image1 = [
    [0, 0, 0, 1, 1, 1, 1, 0, 0, 0],
    [0, 0, 1, 1, 1, 1, 1, 1, 0, 0],
    [0, 0, 1, 1, 0, 0, 1, 1, 0, 0],
    [0, 0, 1, 1, 0, 0, 1, 1, 0, 0],
    [0, 0, 1, 1, 0, 0, 1, 1, 0, 0],
    [0, 0, 1, 1, 1, 1, 1, 1, 0, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [1, 1, 0, 0, 0, 0, 0, 0, 1, 1],
    [1, 1, 0, 0, 0, 0, 0, 0, 1, 1],
    [1, 1, 0, 0, 0, 0, 0, 0, 1, 1],
]

image2 = [
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
    [0, 1, 1, 0, 0, 0, 0, 1, 1, 0],
]

image3 = [
    [1, 1, 0, 0, 0, 0, 0, 0, 1, 1],
    [1, 1, 1, 0, 0, 0, 0, 1, 1, 1],
    [0, 1, 1, 1, 0, 0, 1, 1, 1, 0],
    [0, 0, 1, 1, 1, 1, 1, 1, 0, 0],
    [0, 0, 0, 1, 1, 1, 1, 0, 0, 0],
    [0, 0, 0, 1, 1, 1, 1, 0, 0, 0],
    [0, 0, 1, 1, 1, 1, 1, 1, 0, 0],
    [0, 1, 1, 1, 0, 0, 1, 1, 1, 0],
    [1, 1, 1, 0, 0, 0, 0, 1, 1, 1],
    [1, 1, 0, 0, 0, 0, 0, 0, 1, 1],
]


images = [np.array(image1).reshape(1, 100), np.array(image2).reshape(1, 100), np.array(image3).reshape(1, 100)]


def create_w_matrix():
    ws = np.zeros(shape=(100, 100))
    for i in range(len(images)):
        images[i] = np.where(images[i] == 0, -1, images[i])
        ws += images[i].reshape(100, 1).dot(images[i])
        for j in range(len(ws)):
            ws[j][j] = 0
    return ws


def find_image(w, y):
    y = y.reshape(100, 1).copy()
    for _ in range(100_000):

        res = w.dot(y)
        # print(res)
        res = np.where(res < 0, -1, 1)
        # print(res)
        for image in images:
            if np.array_equal(res.flatten(), image.flatten()):
                return image
        y = res
    return None


def create_noise(per, image):
    im = image.copy().flatten()
    noise_indexes = random.sample(range(len(im)), per)

    for i in noise_indexes:
        im[i] = im[i] * -1
    return im


def print_matrix(mx):
    for i in mx:
        for j in i:
            print(j, end='')
        print()


def print_images(noise, source, per):
    res = np.where(noise == -1, ' ', 0).reshape(10, 10)
    print(f"Noise is {per}%")
    print_matrix(res)
    print("-" * 128)
    if source is None:
        print("Bad News")
        print("-" * 128)
        print("-" * 128)
        return
    source = np.where(source.reshape(10, 10) == -1, ' ', 0)

    print_matrix(source)
    print("-" * 128)
    print("-" * 128)


def main():
    w = create_w_matrix()
    for i in range(0, 40, 10):
        noise = create_noise(i, images[0])
        im = find_image(w, noise)
        print_images(noise, im, i)
    for i in range(0, 40, 10):
        noise = create_noise(i, images[1])
        im = find_image(w, noise)
        print_images(noise, im, i)
    for i in range(0, 50, 10):
        noise = create_noise(i, images[2])
        im = find_image(w, noise)
        print_images(noise, im, i)


if __name__ == '__main__':
    main()
