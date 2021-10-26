import math
from PIL import Image, ImageDraw

number = 50
maxImageHeight = 300
rotateFactor = 30


def GetObjectProperties(labels, areas, imagePath):
    global number
    global rotateFactor
    orientation = 0

    elements = []

    size = len(areas)

    for indexAreas in range(0, size, 1):
        area = GetObjectArea(labels, areas, indexAreas)
        if area < number:
            continue
        perimeter = GetObjectPerimeter(labels, areas, indexAreas)
        compactness = GetObjectCompactness(perimeter, area)

        CreateImage(labels, areas[indexAreas], indexAreas, "Source Images\\")

        xCenter, yCenter = GetObjectCenterOfMass(labels, areas[indexAreas], indexAreas)
        xCenter, yCenter, orientation = GetNeedCenterOfMass(xCenter, yCenter, areas[indexAreas])
        staticMoment11 = GetObjectStaticMoment(labels, areas[indexAreas], indexAreas, xCenter, yCenter, 1, 1, orientation)
        staticMoment20 = GetObjectStaticMoment(labels, areas[indexAreas], indexAreas, xCenter, yCenter, 2, 0, orientation)
        staticMoment02 = GetObjectStaticMoment(labels, areas[indexAreas], indexAreas, xCenter, yCenter, 0, 2, orientation)
        elongation = GetObjectElongation(staticMoment02, staticMoment20, staticMoment11)

        scalingFactor = GetScalingFactor(areas[indexAreas])

        scalingXCenter = scalingFactor * xCenter
        scalingYCenter = scalingFactor * yCenter

        WriteData(str(indexAreas) + ".txt", areas[indexAreas], area, perimeter, compactness, xCenter, yCenter,
                  staticMoment02, staticMoment11, staticMoment20, elongation, scalingXCenter, scalingYCenter)

        elements.append([indexAreas, scalingXCenter, scalingYCenter, compactness * 2, elongation])

    return elements


def GetObjectArea(labels, areas, indexAreas):
    area = 0

    for i in range(areas[indexAreas][0], areas[indexAreas][2] + 1, 1):
        for j in range(areas[indexAreas][1], areas[indexAreas][3] + 1, 1):
            if (labels[i][j] == indexAreas + 1):
                area += 1

    return area


def GetLabel(labels, i, j):
    try:
        return labels[i][j]
    except Exception as exc:
        return 0


def GetObjectPerimeter(labels, areas, indexAreas):
    perimeter = 0

    for i in range(areas[indexAreas][0], areas[indexAreas][2] + 1, 1):
        for j in range(areas[indexAreas][1], areas[indexAreas][3] + 1, 1):
            if labels[i][j] == indexAreas + 1:
                counter = GetLabel(labels, i - 1, j) + GetLabel(labels, i + 1, j) + GetLabel(labels, i,
                                                                                             j - 1) + GetLabel(labels,
                                                                                                               i, j + 1)

                if counter != (indexAreas + 1) * 4:
                    perimeter += 1

    return perimeter


def GetObjectCompactness(perimeter, area):
    return perimeter ** 2 / area


def GetObjectCenterOfMass(labels, areas, indexAreas):
    xCounter = 0
    yCounter = 0
    counter = 0

    for i in range(areas[0], areas[2] + 1, 1):
        for j in range(areas[1], areas[3] + 1, 1):
            if labels[i][j] == indexAreas + 1:
                xCounter += i
                yCounter += j
                counter += 1

    return (xCounter / counter) - areas[0], (yCounter / counter) - areas[1]


def GetObjectStaticMoment(labels, areas, indexAreas, xCenter, yCenter, i, j, orientation):
    staticMoment = 0
    height = areas[2] - areas[0] + 1
    width = areas[3] - areas[1] + 1
    for x in range(areas[0], areas[2] + 1, 1):
        for y in range(areas[1], areas[3] + 1, 1):
            if labels[x][y] == indexAreas + 1:
                valueX = x - areas[0]
                valueY = y - areas[1]
                if orientation == 0:
                    staticMoment += ((valueX - xCenter) ** i) * ((valueY - yCenter) ** j)
                elif orientation == 1:
                    staticMoment += (((height - valueX) - xCenter) ** i) * (((width - valueY) - yCenter) ** j)
                elif orientation == 2:
                    staticMoment += (((width - valueY) - xCenter) ** i) * ((valueX - yCenter) ** j)
                elif orientation == 3:
                    staticMoment += ((valueY - xCenter) ** i) * (((height - valueX) - yCenter) ** j)

    return staticMoment


def GetObjectElongation(staticMoment02, staticMoment20, staticMoment11):
    try:
        part1 = staticMoment20 + staticMoment02
        part2 = math.sqrt((staticMoment20 - staticMoment02) ** 2 + 4 * staticMoment11)

        return (part1 + part2) / (part1 - part2)

    except Exception as exp:
        return math.inf


def GetObjectOrientation(staticMoment02, staticMoment20, staticMoment11):
    try:
        value = 2 * staticMoment11 / (staticMoment20 - staticMoment02)
        return 0.5 * math.atan(value) * 180 / math.pi
    except Exception as exp:
        return 45


def WriteData(fileName, diap, area, perimeter, comp, x, y, stM02, stM20, stM11, el, scalingXCenter,
              scalingYCenter):
    try:
        file = open("Data\\" + fileName, 'w')
        try:
            file.write("Площадь: " + str(area) + "\n")
            file.write("Периметр: " + str(perimeter) + "\n")
            file.write("Компактность: " + str(comp) + "\n")
            file.write("Центр масс x=" + str(x) + " y=" + str(y) + "\n")
            file.write("Статические моменты:\n")
            file.write("stM02=" + str(stM02) + "\n")
            file.write("stM11=" + str(stM11) + "\n")
            file.write("stM20=" + str(stM20) + "\n")
            file.write("Удлинненность: " + str(el) + "\n")
            file.write("Высота: " + str(diap[2] - diap[0] + 1) + "\n")
            file.write("Ширина: " + str(diap[3] - diap[1] + 1) + "\n")
            file.write("Маштабированная координата x центра масс: " + str(scalingXCenter) + "\n")
            file.write("Маштабированная координата y центра масс: " + str(scalingYCenter) + "\n")

        except Exception as ex:
            print(ex)
        finally:
            file.close()
    except Exception as ex:
        print(ex)


def CreateImage(labels, areas, indexAreas, repository):
    height = areas[2] - areas[0] + 1
    width = areas[3] - areas[1] + 1

    image = Image.new("RGB", (height, width))
    draw = ImageDraw.Draw(image)

    for i in range(0, height, 1):
        for j in range(0, width, 1):
            draw.point((i, j), (0, 0, 0))

    for i in range(0, height, 1):
        for j in range(0, width, 1):
            if labels[i + areas[0]][j + areas[1]] == indexAreas + 1:
                draw.point((i, j), (255, 255, 255))

    for i in range(0, height, 1):
        for j in range(0, width, 1):
            if labels[i + areas[0]][j + areas[1]] == 0:
                counter = GetLabel(labels, i - 1 + areas[0], j + areas[1]) + GetLabel(labels, i + 1 + areas[0],
                                                                                      j + areas[1]) + GetLabel(labels,
                                                                                                               i +
                                                                                                               areas[0],
                                                                                                               j - 1 +
                                                                                                               areas[
                                                                                                                   1]) + GetLabel(
                    labels, i + areas[0], j + 1 + areas[1])
                if counter >= 4 * (indexAreas + 1):
                    draw.point((i, j), (255, 255, 255))

    image.save("Images\\" + repository + str(indexAreas) + ".png")


def GetNeedCenterOfMass(xCenter, yCenter, newAreas):
    height = newAreas[2] - newAreas[0] + 1
    width = newAreas[3] - newAreas[1] + 1

    diapX = []
    diapY = []

    diapX.append(xCenter)
    diapX.append(height - xCenter)
    diapX.append(width - yCenter)
    diapX.append(yCenter)

    diapY.append(yCenter)
    diapY.append(width - yCenter)
    diapY.append(xCenter)
    diapY.append(height - xCenter)

    maxValue = max(diapX)
    orientation = diapX.index(maxValue)

    return diapX[orientation], diapY[orientation], orientation


def GetScalingFactor(areas):
    global maxImageHeight

    height = areas[2] - areas[0] + 1
    width = areas[3] - areas[1] + 1

    value = max(height, width)

    return maxImageHeight / value

