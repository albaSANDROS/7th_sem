import math
from PIL import Image, ImageDraw

imageHeight = 0
imageWidth = 0
flagL = False


def Labeling(path, img):
    global imageHeight
    global imageWidth
    global flagL

    image = Image.open(path)

    imageHeight = image.size[0]
    imageWidth = image.size[1]

    L = 0
    areas = []
    flagL = True

    labels = [[0] * imageWidth for i in range(imageHeight)]

    for i in range(0, imageHeight, 1):
        for j in range(0, imageWidth, 1):
            if flagL == True:
                L += 1
                areas.append([int(i), int(j), int(i), int(j)])
                flagL = False
            else:
                areas[L - 1][0] = areas[L - 1][2] = i
                areas[L - 1][1] = areas[L - 1][3] = j

            Fill(img, labels, L, i, j, areas)

    if flagL == False:
        areas.pop(L - 1)

    return labels, areas


def Fill(img, labels, L, i, j, areas):
    global flagL
    if labels[i][j] == 0 and img[i][j] == 1:
        labels[i][j] = L
        flagL = True

        if i < areas[L - 1][0]:
            areas[L - 1][0] = i
        elif i > areas[L - 1][2]:
            areas[L - 1][2] = i

        if j < areas[L - 1][1]:
            areas[L - 1][1] = j
        elif j > areas[L - 1][3]:
            areas[L - 1][3] = j

        if j > 0:
            Fill(img, labels, L, i, j - 1, areas)

        if j < imageWidth - 1:
            Fill(img, labels, L, i, j + 1, areas)

        if i > 0:
            Fill(img, labels, L, i - 1, j, areas)

        if i < imageHeight - 1:
            Fill(img, labels, L, i + 1, j, areas)

