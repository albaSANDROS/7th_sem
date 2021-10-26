import ImageDissection
import ObjectProperties
import ObjectSelection
import ClusterAnalysis
import sys

def WriteClusterIndexes(clusters):
    try:
        file = open("Data\\clusters.txt", 'w')
        try:
            for i in range(0, len(clusters), 1):
                file.write(str(i + 1) + "-й кластер: ")
                for j in range(0, len(clusters[i]), 1):
                    file.write(str(clusters[i][j]) + "; ")
                file.write("\n")
        except Exception as ex:
            print(ex)
        finally:
            file.close()
    except Exception as ex:
        print(ex)


def main():
    sys.setrecursionlimit(5000)
    while 1:
        try:
            path = input("Enter the path to the image: ")
            if path[0] == "\"":
                path = path[1:]
            if path[-1] == "\"":
                path = path[:-1]
            break
        except Exception as exp:
            print("Error: " + str(exp))
    while 1:
        try:
            min = int(input("Enter the min value for dissection: "))

            if 0 > min or min > 255:
                raise Exception("Min value cannot be less than zero or greater than 255")
            break
        except Exception as exp:
            print("Error: " + str(exp))

    while 1:
        try:
            max = int(input("Enter the max value for dissection: "))

            if min > max or max > 255:
                raise Exception("Max value cannot be less than min value or greater than 255")
            break
        except Exception as exp:
            print("Error: " + str(exp))

    image, img = ImageDissection.Dissection(min, max, path)

    image.save("D:\\Projects\\Cosii2\\result_images\\DissectedImage.png", )

    labels, areas = ObjectSelection.Labeling(path, img)

    elements = ObjectProperties.GetObjectProperties(labels, areas, path)
    size = len(elements)

    while 1:
        try:
            print("Enter the number of clusters")
            numberOfClusters = int(input())

            if numberOfClusters < 2 or numberOfClusters > size:
                raise Exception(
                    "Number of clusters cannot be less than two or greater than number of objects in the photo")
            break
        except Exception as exp:
            print("Error: " + str(exp))

    clusters = ClusterAnalysis.GetClusters(elements, numberOfClusters)

    WriteClusterIndexes(clusters)

main()

