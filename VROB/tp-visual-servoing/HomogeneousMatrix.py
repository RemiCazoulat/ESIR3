# Copyright 2023 Eric Marchand Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np
import math

import ThetaUVector

class HomogeneousMatrix:
  def __init__(self, mat):
    # check if input is a 4x4 matrix
    if mat.shape != (4, 4):
      raise ValueError("Input matrix must be 4x4")
    self.matrix = mat
  # def __init__(self):
  #   mat = np.eye(4)
  #   self.matrix = mat
  # function to print the matrix
  def __repr__(self):
    return f"HomogeneousMatrix({self.matrix})"

  # function to multiply two homogeneous matrices
  def __mul__(self, other):
    if isinstance(other, HomogeneousMatrix): # T12 = (R1R2  R1*t2+t1)
      R1 = self.matrix[:3, :3]
      R2 = other.matrix[:3, :3]
      t1 = self.matrix[:3, 3:]
      t2 = other.matrix[:3, 3:]
      R12 = np.matmul(R1, R2)
      t12 = np.matmul(R1, t2) + t1
      mat= np.block([[R12, t12], [np.zeros((1, 3)), 1]])
      return HomogeneousMatrix(mat)
    elif isinstance(other, np.ndarray) and other.shape == (4,):
      # transform a 3D point using the homogeneous matrix
      transformed = np.matmul(self.matrix, other)
      return  transformed # convert back to 3D coordinates
    elif isinstance(other, np.ndarray) and other.shape == (3,):
      # transform a 3D point using the homogeneous matrix
      p = np.append(other, 1) # append a 1 to create a homogeneous coordinate
      transformed = np.matmul(self.matrix, p)
      return transformed[:3] / transformed[3] # convert back to 3D coordinates
    else:
      print("Error in multiplication")
      raise TypeError(f'Multiplication with type {type(other)} not supported')

  def extractRotation(self):
    R = self.matrix[:3, :3]
    return R

  def extractTranslation(self):
    t = self.matrix[:3, 3:]
    return t

  # function to inverse the homogeneous matrix  (R t)^-1 = (R^T -R^T*t)
  def inverse(self):
    R = self.matrix[:3, :3]
    t = self.matrix[:3, 3:]
    inv_R = R.T
    inv_t = -np.matmul(inv_R, t)
    inv_mat = np.block([[inv_R, inv_t], [np.zeros((1, 3)), 1]])
    return HomogeneousMatrix(inv_mat)

  # function to insert a 3x3 rotation matrix into a 4x4 homogeneous matrix
  def insertRotation(self, R):
    #insert a 3x3 rotation matrix into a 4x4 homogeneous matrix
    self.matrix[:3, :3] = R #.matrix[:3, :3]
    return self

  # function to insert a 3x1 translation vector into a 4x4 homogeneous matrix
  def insertTranslation(self, t):
    self.matrix[:3, 3:] = t
    return self

  # extract the theta u from the rotation matrix
  def extractThetaU(self):
    R = self.matrix[:3, :3]
    thetau = ThetaUVector.buildFrom(R)
    return thetau

  # function to convert the homogeneous matrix to a 6x1 vector
  def toVector(self):
    thetau = self.extractThetaU()
    t = self.matrix[:3, 3:]
    return np.append(thetau, t)



