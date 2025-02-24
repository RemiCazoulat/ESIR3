# Copyright 2023 Eric Marchand Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np
import math
import sys

import Math

def buildFrom(R):
  """! Build an angle-axis minimal representation from a rotation matrix.
  @param[in] R 3x3 rotation matrix as a numpy array. .
  @return 3-dim numpy vector that corresponds to the angle-axis minimal representation which contains `(thetau_x, thetau_y, thetau_z)`.
  """

  s, c, theta = 0.0, 0.0, 0.0

  s = ((R[1][0] - R[0][1]) ** 2 + (R[2][0] - R[0][2]) ** 2 + (R[2][1] - R[1][2]) ** 2) ** 0.5 / 2.0
  c = (np.trace(R) - 1.0) / 2.0
  theta = math.atan2(s, c) # theta in [0, PI] since s > 0

  thetau = np.array([0.,0.,0.]) # thetau is the rotation vector

  minimum = 0.0001
  # General case when theta != pi. If theta=pi, c=-1
  if (1 + c) > minimum: # Since -1 <= c <= 1, no fabs(1+c) is required
    sc = Math.sinc(theta)
    #print("sinc = ", sc)
    #print(R)
    #print(R[2][1])

    thetau = np.array([(R[2][1] - R[1][2]) / (2 * sc),
                        (R[0][2] - R[2][0]) / (2 * sc),
                        (R[1][0] - R[0][1]) / (2 * sc)])

  else: # theta near PI
    x = 0
    if (R[0][0] - c) > sys.float_info.epsilon:
      x = math.sqrt((R[0][0] - c) / (1 - c))

    y = 0
    if (R[1][1] - c) > sys.float_info.epsilon:
      y = math.sqrt((R[1][1] - c) / (1 - c))

    z = 0
    if (R[2][2] - c) > sys.float_info.epsilon:
      z = math.sqrt((R[2][2] - c) / (1 - c))

    if x > y and x > z:
      if (R[2][1] - R[1][2]) < 0:
        x = -x
      if Math.sign(x) * Math.sign(y) != Math.sign(R[0][1] + R[1][0]):
        y = -y
      if Math.sign(x) * Math.sign(z) != Math.sign(R[0][2] + R[2][0]):
        z = -z
    elif y > z:
      if (R[0][2] - R[2][0]) < 0:
        y = -y
      if Math.sign(y) * Math.sign(x) != Math.sign(R[1][0] + R[0][1]):
        x = -x
      if Math.sign(y) * Math.sign(z) != Math.sign(R[1][2] + R[2][1]):
        z = -z
    else:
      if (R[1][0] - R[0][1]) < 0:
        z = -z
      if Math.sign(z) * Math.sign(x) != Math.sign(R[2][0] + R[0][2]):
        x = -x
      if Math.sign(z) * Math.sign(y) != Math.sign(R[2][1]+R[1][2]):
        y = -y

    thetau[0] = theta*x
    thetau[1] = theta*y
    thetau[2] = theta*z

  return thetau
