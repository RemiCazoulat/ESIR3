import scipy.io
import matplotlib.pyplot as plt
import numpy as np
import pyvista as pv
from constructMatrixOfMonomials import *
from constructSetOf321Polynomials import *
from lsqnoneg import *
from scipy.optimize import nnls
import time
import cvxpy as cp

def printTensor(tensor, order):
    c = 0
    for i in range(0, order):
        for j in range(0, order - i):
            print(f"D{i}{j}{order - i - j} = {tensor[c]:.8f}")
            c = c + 1


def computeDTI(voxel, order=2, dir_number=6, S0=1.0, print_tensor=False):
    # Initialisation des variables

    b_value = np.array(voxel[0] * dir_number)
    S = []
    for value in range(1, dir_number):
        S = S + [voxel[value]]
    GradientOrientations = np.array([
        [-1, 0, 1],
        [1, 0, 1],
        [0, 1, 1],
        [0, 1, -1],
        [-1, 1, 0],
        [1, 1, 0]
    ])
    # Calcul de G et C
    G = constructMatrixOfMonomials(GradientOrientations, order)
    C = construct_set_of_321_polynomials(order).T  # Transposition pour correspondre à MATLAB

    print(C)
    print(G)

    # Construction de P
    print(len(G))
    print(len(G[0]))

    print(len(C))
    print(len(C[0]))
    P = G @ C  # Multiplication matricielle
    P = -np.diag(b_value) @ P

    cond_P = np.linalg.cond(P)
    print(f"Condition number of P: {cond_P}")
    # Début du chronométrage
    start_time = time.time()

    # Résolution des moindres carrés non négatifs
    y = np.log(S / S0)
    print(f"P shape: {P.shape}")
    print(f"y shape: {y.shape}")

    x_var = cp.Variable(P.shape[1], nonneg=True)
    objective = cp.Minimize(cp.norm(P @ x_var - y, 2))
    problem = cp.Problem(objective)
    problem.solve()

    x = x_var.value

    # Calcul des coefficients du tenseur
    UniqueTensorCoefficients = C @ x

    # Fin du chronométrage
    end_time = time.time()
    print(f"\nTotal estimation time: {int((end_time - start_time) * 1000)} ms\n")

    # Affichage des résultats
    if print_tensor:
        printTensor(UniqueTensorCoefficients, order)

    # Mise en forme du résultat en matrice 3x3
    D = np.array([
        [UniqueTensorCoefficients[5], UniqueTensorCoefficients[4] / 2, UniqueTensorCoefficients[3] / 2],
        [UniqueTensorCoefficients[4] / 2, UniqueTensorCoefficients[2], UniqueTensorCoefficients[1] / 2],
        [UniqueTensorCoefficients[3] / 2, UniqueTensorCoefficients[1] / 2, UniqueTensorCoefficients[0]]
    ])
    print("\n3x3 Tensor Matrix (D):")
    print(D)
    return D
