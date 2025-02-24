import numpy as np
import csv


def population_basis(i, order, c):
    """
    Recursive helper function for population to compute basis coefficients.

    Args:
        i (int): Current index
        order (int): Remaining order
        c (list): Current coefficient list

    Returns:
        list: Coefficients [x, y, z]
    """
    if order == 0:
        return c
    else:
        j = i % 3
        c[j] += 1
        return population_basis(i // 3, order - 1, c)


def population(i, j, k, order):
    """
    Computes how many identical copies of a tensor coefficient appear in
    a fully symmetric high-order tensor in 3 variables of specific order.

    Args:
        i (int): Exponent of x
        j (int): Exponent of y
        k (int): Exponent of z
        order (int): Order of the symmetric tensor in 3 variables (x, y, z)

    Returns:
        int: Count of identical tensor coefficients
    """
    size = 3 ** order
    counter = 0
    for z in range(size):
        c = population_basis(z, order, [0, 0, 0])
        if c[0] == i and c[1] == j and c[2] == k:
            counter += 1
    return counter


def construct_set_of_321_polynomials(order):
    print("Constructing matrix of 321 polynomials C...")

    # Load UnitVectors from the CSV file
    g = np.loadtxt("unit_vectors.csv", delimiter=',')
    Mprime = 321
    g = g[:Mprime, :]  # Select the first 321 vectors

    # Initialize the `pop` array
    pop = np.zeros((order + 1, order + 1, order + 1))

    for i in range(order + 1):
        for j in range(order + 1 - i):
            pop[i, j, order - i - j] = population(i, j, order - i - j, order)

    # Compute the matrix C
    C = np.zeros((len(g), int((2 + order) * (1 + order) / 2)))  # Preallocate C

    for k in range(len(g)):
        c = 0  # Column index starts at 0
        for i in range(order + 1):
            for j in range(order + 1 - i):
                coeff = pop[i, j, order - i - j] * (g[k, 0] ** i) * (g[k, 1] ** j) * (g[k, 2] ** (order - i - j))
                C[k, c] = coeff
                c += 1

    print("Done")
    return C
