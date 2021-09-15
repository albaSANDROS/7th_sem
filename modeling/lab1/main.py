import numpy as np
import matplotlib.pyplot as plt

sequence = []


def made_sequence(a, Rn, m, N):
    for x in range (0, N):
        Rn = a * Rn % m
        sequence.append(Rn/m)

def show_histogram():
    weights = np.ones_like(sequence) / float(len(sequence))
    plt.hist(sequence, bins=np.linspace(0, 1, 21), weights=weights, histtype='bar', color='blue', rwidth=0.95)
    plt.hlines(1/20, 0, 1)
    plt.show()


def indirect_check():
    amount_of_K_pairs = 0
    for i in range(0, len(sequence), 2):
        if sequence[i] ** 2 + sequence[i + 1] ** 2 < 1:
            amount_of_K_pairs += 1
    print('\nОпытный: ', 2 * amount_of_K_pairs / len(sequence), '\nТеоретический: ', np.pi / 4)


def period_length():
    indexes = []
    for i in range(0, len(sequence)):
        if sequence[i] == sequence[-1]:
            indexes.append(i)
    if len(indexes) == 1:
        indexes.append(indexes[0])
    return indexes[1] - indexes[0]


def aperiodic_section_length(P):
    index = 0
    while sequence[index] != sequence[index + P]:
        index += 1
        if (index + P == len(sequence) - 1):
            StopIteration

    return index + P

if __name__ == '__main__':

    print('\nУсловия: R0, A, M - положительные значения;\n         M обязательно больше чем A;\n')
    R = int(input("R0:"))
    a = int(input("a: "))
    m = int(input("m: "))
    if R < 0:
        print('\nВы вводите отрицательный R')
    elif a < 0:
        print('\nВы вводите отрицательный a')
    elif m < 0:
        print('\nВы вводите отрицательный m')
    elif m < a:
        print('\nm должен быть больше чем a')
    else:
        made_sequence(a, R, m, 1000000)
        print('\nМатематическое ожидание: ', np.mean(sequence), '\nДисперсия: ', np.var(sequence), '\nСреднеквадратичное отклонениеS: ', np.std(sequence))
        indirect_check()
        period = period_length()
        aperiodic_section = aperiodic_section_length(period)
        print('\nПериод: ', period, '\nАпериодический интервал: ', aperiodic_section)
        show_histogram()

# a = 134279
# r0 = 1
# m = 313107
# n = 1000000
