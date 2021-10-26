from pip import Image,ImageDraw

def Correction(minBrightness, maxBrightness,path):
    image = Image.open(path)
    draw = ImageDraw.Draw(image)

    height = image.size[0]
    width = image.size[1]

    pix = image.load()

    range_ = GetMinMaxBrightness(image)

    a = (maxBrightness - minBrightness)/(range_[1] - range_[0])
    b = maxBrightness - a * range_[1]

    for i in range(height):
        for j in range(width):
            draw.point((i,j), (int(a * pix[i,j][0] + b), int(a * pix[i,j][1] + b), int(a * pix[i,j][2] + b)))
            
    return image

def GetMinMaxBrightness(image):
    height = image.size[0]
    width = image.size[1]

    pix = image.load()

    range_ = [float(0.3 * pix[0,0][0] + 0.59 * pix[0,0][1] + 0.11 * pix[0,0][2]),float(0.3 * pix[0,0][0] + 0.59 * pix[0,0][1] + 0.11 * pix[0,0][2])]


    for i in range(height):
        for j in range(width):
            value = int(0.3 * pix[i,j][0] + 0.59 * pix[i,j][1] + 0.11 * pix[i,j][2])
            if value < range_[0]:
                range_[0] = value
            elif value > range_[1]:
                range_[1] = value

    return range_



