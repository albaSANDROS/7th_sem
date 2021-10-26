import math
numberOfCompanents = 4

def IsCenterOfCluster(centersOfClusters, index):
    try:
        value = centersOfClusters.index(index)
        return True, value
    except ValueError:
        return False, -1

def GetClusters(elements, numberOfClusters):
    IsClustersEquals = False
    oldClusters = []

    centersOfClusters = [int(0)] * numberOfClusters

    for i in range(0,numberOfClusters,1):
        centersOfClusters[i] = i

    while IsClustersEquals == False:
        clusters = []

        for i in range(0,numberOfClusters,1):
            newClusters = []
            clusters.append(newClusters)

        for i in range(0,len(elements),1):
            flag,ind = IsCenterOfCluster(centersOfClusters, i)
            if flag == True:
                clusters[ind].append(i)
                continue

            distance = [float(0)] * numberOfClusters

            for j in range(0, numberOfClusters, 1):
                distance[j] = GetObjectsDistance(elements,centersOfClusters[j], i)

            index = distance.index(min(distance))
            clusters[index].append(i)

        IsClustersEquals = IsEquals(oldClusters, clusters)

        oldClusters = clusters

        centersOfClusters = GetNewCenterOfClusters(elements,clusters)

    GetElementsIndexOfClusters(elements,oldClusters)

    return oldClusters

def GetElementsIndexOfClusters(elements,oldClusters):
    for i in range(0, len(oldClusters), 1):
        for j in range(0, len(oldClusters[i]), 1):
            oldClusters[i][j] = int(elements[oldClusters[i][j]][0])


def GetObjectsDistance(elements, centerIndex, objectIndex):
    value = 0

    for i in range(1,5,1):
        value += (elements[centerIndex][i] - elements[objectIndex][i]) ** 2

    return math.sqrt(value)

def CopyClusters(clusters):
    newClusters = []

    for i in range(0, len(clusters), 1):
        newCl = []
        newClusters.append(newCl)

        for j in range(0, len(clusters[i]), 1):
            newClusters[i].append(clusters[i][j])

    return newClusters

def IsEquals(oldClusters, newClusters):
    if oldClusters == None or len(oldClusters) != len(newClusters):
        return False

    for i in range(0, len(oldClusters), 1):
        if len(oldClusters[i]) != len(newClusters[i]):
            return False

        for j in range(0, len(oldClusters[i]), 1):
            if oldClusters[i][j] != newClusters[i][j]:
                return False

    return True

def GetNewCenterOfClusters(elements,clusters):
    global numberOfCompanents
    centersOfClusters = []

    for i in range(0, len(clusters), 1):
        distance = [float(0)] * (len(clusters[i]))

        for j in range(0, len(clusters[i]), 1):
            for z in range (0, len(clusters[i]), 1):
                distance[j] += GetObjectsDistance(elements,clusters[i][j],clusters[i][z])

        index = distance.index(min(distance))

        centersOfClusters.append(clusters[i][index])

    return centersOfClusters 

