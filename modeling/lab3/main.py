import numpy
import random


def simulate_teoriya():
    q1 = 0.6
    q2 = 0.6
    #q2 = 1
    p1 = 1 - q1
    p2 = 1 - q2

    # some defines for clearness
    pp = q1 * q2
    pq = q1 * p2
    qp = p1 * q2
    qq = p1 * p2

    Matrix = numpy.array([[1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.],  # P1
                          [1., -1., q2, 0., 0., 0., 0., 0., 0., 0., 0., 0.],  # P2
                          [0., q1, -1., 0., pp, 0., 0., 0., 0., 0., 0., 0.],  # P3
                          [0., p1, 0., -1., qp, 0., 0., 0., 0., 0., 0., 0.],  # P4
                          [0., 0., p2, q1, -1., q1, q2, pp, 0., pp, 0., 0.],  # P5
                          [0., 0., 0., p1, 0., (p1 - 1), 0., qp, 0., qp, 0., 0.],  # P6
                          [0., 0., 0., 0., pq, 0., -1., 0., (pp + pq), 0., 0., 0.],  # P7
                          [0., 0., 0., 0., qq, 0., 0., -1., qp, 0., 0., 0.],  # P8
                          [0., 0., 0., 0., 0., 0., p2, pq, -1., pq, (pp + pq), (pp + pq)],  # P9
                          [0., 0., 0., 0., 0., 0., 0., qq, 0., (qq - 1), qp, qp],  # P10
                          [0., 0., 0., 0., 0., 0., 0., 0., qq, 0., -1., 0.],  # P11
                          [0., 0., 0., 0., 0., 0., 0., 0., 0., 0., qq, (qq - 1)]  # P12
                          ])

    Vector = numpy.array([1., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.])
    P = numpy.linalg.solve(Matrix, Vector)
    print('Teoria: \n\n')

    for i in range(len(P) - 1): print('P' + str(i + 1) + ': ' + str(P[i + 1]))
    print(sum(P))
    print('\n')
    P1 = P[0]
    P2 = P[1]
    P3 = P[2]
    P4 = P[3]
    P5 = P[4]
    P6 = P[5]
    P7 = P[6]
    P8 = P[7]
    P9 = P[8]
    P10 = P[9]
    P11 = P[10]
    P12 = P[11]

    Pbl = P6 + P10 + P12
    print('P bl: ', Pbl)

    K1 = (P2 + P4 + P5 + P6 + P8 + P9 + P10 + P11 + P12)
    print('K1: ', K1)

    K2 = (P3 + P5 + P7 + P8 + P9 + P10 + P11 + P12)
    print('K2: ', K2)

    A = (q2 * K2)
    print('A: ', A)

    q = A / (0.5 * (1 - Pbl))
    print('Q: ', q)

    Potk = 1 - q
    print('P otk: ', Potk)

    Lo = (P7 + P9 + P11 + P12)
    print('L o: ', Lo)

    Lc = (P2 + P3 + P4 + 2 * P5 + 2 * P6 + 2 * P7 + 2 * P8 + 3 * P9 + 3 * P10 + 3 * P11 + 4 * P12)
    print('L c: ', Lc)

    Wc = Lc / (0.5 * (1 - Pbl))
    print('W c: ', Wc)

    Wo = Lo / (0.5 * (1 - Pbl))
    print('W o: ', Wo)


def simulate_praktika():
    Times = 1_000_000


    P1_v = 0.4
    P2_v = 0.4



    state = "1000"  # P1

    P1000 = 0
    P2100 = 0
    P1001 = 0
    P1100 = 0
    P2101 = 0
    P0100 = 0
    P1011 = 0
    P1101 = 0
    P2111 = 0
    P0101 = 0
    P1111 = 0
    P0111 = 0

    A = 0
    K1 = 0
    K2 = 0
    Lc = 0
    Lo = 0
    Pbl = 0
    gen = 0

    for i in range(Times):
        q1 = random.uniform(0.0, 1) >= P1_v
        q2 = random.uniform(0.0, 1) >= P2_v
        #q2= 1 
        if (state[0] == '0'):
            Lc += 1
            Pbl += 1

        if (state[1] == '1'):
            Lc += 1
            K1 += 1

        if (state[2] == '1'):
            Lc += 1
            Lo += 1

        if (state[3] == '1'):
            Lc += 1
            K2 += 1
            if (q2):
                A += 1

        # P1
        if (state == "1000"):
            P1000 += 1
            gen += 1
            state = "2100"
            continue

        # P2
        if (state == "2100"):
            P2100 += 1
            if (q1):  # q1
                state = "1001"
            if (not q1):  # p1
                state = "1100"
            continue

        # P3
        if (state == "1001"):
            P1001 += 1
            gen += 1
            if (q2):  # t
                state = "2100"
            if (not q2):  # p2
                state = "2101"
            continue

        # P4
        if (state == "1100"):
            P1100 += 1
            gen += 1
            if (q1):  # q1
                state = "2101"
            if (not q1):  # p1
                state = "0100"
            continue

        # P5
        if (state == "2101"):
            P2101 += 1
            if (q1 and q2):  # q1*q2
                state = "1001"
            if (q1 and not q2):  # q1*p2
                state = "1011"
            if (not q1 and q2):  # p1*q2
                state = "1100"
            if (not q1 and not q2):  # p1*p2
                state = "1101"
            continue

        # P6
        if (state == "0100"):
            P0100 += 1
            if (q1):  # q1
                state = "2101"
            if (not q1):  # p1
                state = "0100"
            continue

        # P7
        if (state == "1011"):
            P1011 += 1
            gen += 1
            if (q2):  # q2
                state = "2101"
            if (not q2):  # p2
                state = "2111"
            continue

        # P8
        if (state == "1101"):
            P1101 += 1
            gen += 1
            if (q1 and q2):  # q1*t
                state = "2101"
            if (q1 and not q2):  # q1*p2
                state = "2111"
            if (not q1 and q2):  # p1*t
                state = "0100"
            if (not q1 and not q2):  # p1*p2
                state = "0101"
            continue

        # P9
        if (state == "2111"):
            P2111 += 1
            if (q1 and q2):  # q1*t
                state = "1011"
            if (q1 and not q2):  # q1*p2
                state = "1011"
            if (not q1 and q2):  # p1*t
                state = "1101"
            if (not q1 and not q2):  # p1*p2
                state = "1111"
            continue

        # P10
        if (state == "0101"):
            P0101 += 1
            if (q1 and q2):  # q1*t
                state = "2101"
            if (q1 and not q2):  # q1*p2
                state = "2111"
            if (not q1 and q2):  # p1*t
                state = "0100"
            if (not q1 and not q2):  # p1*p2
                state = "0101"
            continue

        # P11
        if (state == "1111"):
            P1111 += 1
            gen += 1
            if (q1 and q2):  # q1*q2
                state = "2111"
            if (q1 and not q2):  # q1*p2
                state = "2111"
            if (not q1 and q2):  # p1*q2
                state = "0101"
            if (not q1 and not q2):  # p1*p2
                state = "0111"
            continue

        # P12
        if (state == "0111"):
            P0111 += 1
            if (q1 and q2):  # q1*t
                state = "2111"
            if (q1 and not q2):  # q1*p2
                state = "2111"
            if (not q1 and q2):  # p1*t
                state = "0101"
            if (not q1 and not q2):  # p1*p2
                state = "0111"
            continue

    P2 = P2100 / Times
    P3 = P1001 / Times
    P4 = P1100 / Times
    P5 = P2101 / Times
    P6 = P0100 / Times
    P7 = P1011 / Times
    P8 = P1101 / Times
    P9 = P2111 / Times
    P10 = P0101 / Times
    P11 = P1111 / Times
    P12 = P0111 / Times

    print("Simulation praktika: ", "\n")
    ##print("P1: ", P1)
    print("P1: ", P2)
    print("P2: ", P3)
    print("P3: ", P4)
    print("P4: ", P5)
    print("P5: ", P6)
    print("P6: ", P7)
    print("P7: ", P8)
    print("P8: ", P9)
    print("P9: ", P10)
    print("P10: ", P11)
    print("P11: ", P12)

    lambda_ = (gen)

    print("\nPbl: ", (Pbl / Times))

    print("K1: ", K1 / Times)

    print("K2: ", K2 / Times)

    print("A: ", (A / Times))

    print("Q: ", (A / lambda_))

    print("Potk: ", (1 - (A / lambda_)))

    print("Lo: ", (Lo / Times))

    print("Lc: ", (Lc / Times))

    print("Wc: ", (Lc / lambda_))

    print("Wo: ", (Lo / lambda_))


def main():
    simulate_teoriya()
    simulate_praktika()


main()
