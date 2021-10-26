from math import floor

from PIL import Image, ImageDraw

def Dissection(minValue, maxValue, path):
    image = Image.open(path)
    draw = ImageDraw.Draw(image)

    height = image.size[0]
    width = image.size[1]

    img =[[0] * width for i in range(height)]


    pix = image.load()

    for i in range(height):
        for j in range(width):
            value = floor(0.3 * pix[i,j][0] + 0.59 * pix[i,j][1] + 0.11 * pix[i,j][2])
            if minValue <= value and value <= maxValue:
                draw.point((i,j),(255,255,255))
                img[i][j] = 1
            else:
                draw.point((i,j),(0,0,0))

    return image,img
