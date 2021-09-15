import numpy as np
import matplotlib.pyplot as plt
import random
import math
from functools import reduce

def get_random_sequence(n):
    return list(map(lambda x: random.random(), range(0, n)))


def show_hist(sequence):
    weights = np.ones_like(sequence) / float(len(sequence))
    plt.hist(sequence, bins=np.linspace(min(sequence), max(sequence), 21),
             weights=weights, histtype='bar', color='blue', rwidth=0.95)
    plt.show()


def review(sequence):
    show_hist(sequence)
    print('\nMean: ', np.mean(sequence), '\nVariance: ',
          np.var(sequence), '\nStandard deviation: ', np.std(sequence), '\n')

# a + (b - a) * x
def get_uniform_distribution(a, b):
    return list(map(lambda x: a + (b - a) * x, get_random_sequence(1000000)))

# M + std * /(12/n) * (random - n/2)
def get_gauss_distribution(mean, std, n=6):
    return list(map(lambda x: mean + std * math.sqrt(12/n) * (sum(get_random_sequence(n)) - n/2), range(0, 1000000)))

# -1/lambda * ln(R)
def get_exponential_distribution(lambda_param):
    return list(map(lambda x: - 1 / lambda_param * math.log(x), get_random_sequence(1000000)))

# -1/lambda * ln(summ(yz))
def get_gamma_distribution(eta, lambda_param):
    return list(map(lambda x: -1 / lambda_param * math.log(reduce(lambda y, z: y*z, get_random_sequence(eta))),
                    range(0, 1000000)))

#  2 * (b - x) / (b - a)^2
def get_min_triangle_distribution(a, b):
    return list(map(lambda x: a + (b - a) * min(get_random_sequence(2)), range(0, 1000000)))

#  2 * (x - a) / (b - a)^2
def get_max_triangle_distribution(a, b):
    return list(map(lambda x: a + (b - a) * max(get_random_sequence(2)), range(0, 1000000)))

# X = y + z
def get_simpson_distribution(a, b):
    return list(map(lambda x, y: x + y, get_uniform_distribution(a/2, b/2), get_uniform_distribution(a/2, b/2)))

print('Uniform distribution: ')
review(get_uniform_distribution(int(input('A: ')), int(input('B: '))))

print('Gauss distribution: ')
review(get_gauss_distribution(int(input('Mean: ')), int(input('Std: ')), int(input('N: '))))

print('Exponential distribution: ')
review(get_exponential_distribution(int(input('λ: '))))

print('Gamma distribution: ')
review(get_gamma_distribution(int(input('η: ')), int(input('λ: '))))

print('Min-Triangle distribution')
review(get_min_triangle_distribution(int(input('A: ')), int(input('B: '))))

print('Max-Triangle distribution')
review(get_max_triangle_distribution(int(input('A: ')), int(input('B: '))))

print('Simpson distribution')
review(get_simpson_distribution(int(input('A: ')), int(input('B: '))))
