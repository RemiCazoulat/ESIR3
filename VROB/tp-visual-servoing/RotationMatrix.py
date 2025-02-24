# Copyright 2023 Eric Marchand Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np
import math

import Math

def buildFromThetaUVector(thetau):
  """! Build a rotation matrix from an angle-axis minimal representation.
  @param[in] thetau  3-dim numpy vector that contains `(thetau_x, thetau_y, thetau_z)`.
  @return  The corresponding 3x3 rotation matrix.
  """
  v =  np.array([0.0, 0.0, 0.0])

  v = thetau
  theta = math.sqrt(v[0]**2 + v[1]**2 + v[2]**2)
  si = math.sin(theta)
  co = math.cos(theta)
  sc = Math.sinc(theta)
  mcc = Math.mcosc(co,theta)

  R = np.eye(3)

  R[0][0] = co + mcc * v[0]**2
  R[0][1] = -sc * v[2] + mcc * v[0] * v[1]
  R[0][2] = sc * v[1] + mcc * v[0] * v[2]
  R[1][0] = sc * v[2] + mcc * v[1] * v[0]
  R[1][1] = co + mcc * v[1]**2
  R[1][2] = -sc * v[0] + mcc * v[1] * v[2]
  R[2][0] = -sc * v[1] + mcc * v[2] * v[0]
  R[2][1] = sc * v[0] + mcc * v[2] * v[1]
  R[2][2] = co + mcc * v[2]**2

  return R

def inverse(R):
  if R.shape != (3,3):
    raise Exception(f"Rotation matrix has shape {R.shape}, but should be (3,3)")

  inv_R = R.T
  return inv_R
