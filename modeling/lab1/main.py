import numpy as np
import matplotlib.pyplot as plt

sequence = []
def calculate_sequence(a, m, Rn, N):
    for x in range(0, N):
        Rn = a * Rn % m
        sequence.append(Rn / m)


def show_hist():
    weights = np.ones_like(sequence) / float(len(sequence))
    plt.hist(sequence, bins=np.linspace(0, 1, 21), weights=weights, histtype='bar', color='blue', rwidth=0.95)
    plt.hlines(1/20, 0, 1)
    plt.show()


def calculate_implicit_criteria():
    criteria_matchers = 0
    for i in range(0, len(sequence), 2):
        if sequence[i] ** 2 + sequence[i + 1] ** 2 < 1:
            criteria_matchers += 1
    print('\nРеальность: ', 2 * criteria_matchers / len(sequence), '\nОжидание: ', np.pi / 4)


def calculate_period():
    x_v_index_matchers = []
    for i in range(0, len(sequence)):
        if sequence[i] == sequence[-1]:
            x_v_index_matchers.append(i)
    return x_v_index_matchers[1] - x_v_index_matchers[0]


def calculate_aperiodic_inteval(P):
    i_3 = 0
    while sequence[i_3] != sequence[i_3 + P]:
        i_3 += 1
    return P + i_3
# a = 134279
# r0 = 1
# m = 313107
# n = 1000000
R = int(input("R0:"))
a = int(input("a: "))
m = int(input("m: "))
calculate_sequence(a, m, R, 1000000)
print('\nМат.ожидание: ', np.mean(sequence), '\nДисперсия: ', np.var(sequence), '\nОтклонение: ', np.std(sequence))

calculate_implicit_criteria()

period = calculate_period()
aperiodic_interval = calculate_aperiodic_inteval(period)
print('\nПериод: ', period, '\nАпериодический интервал: ', aperiodic_interval)
show_hist()

