# Copyright 2023 Eric Marchand Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np
import math

import Math
import ThetaUVector
import RotationMatrix
from HomogeneousMatrix import HomogeneousMatrix

# Compute the exponential map of a vector
# Return a homogeneous matrix
def direct(v, delta_t):
  """! Compute the exponential map of a vector.
  @param[in] v  Instantaneous velocity skew represented by a 6 dimension vector \f$ {\bf v} = [v, \omega] \f$
                where \f$ v \f$ is a translation velocity vector and \f$ \omega \f$ is a rotation velocity vector.
  @param[in] delta_t  Sampling time in seconds corresponding to the time during which the velocity $ \bf v $ is applied.
  @return  The exponential map of vector `v` corresponding to an homogeneous matrix represented by a 4-by-4 numpy array.
  """
  v_dt = v * delta_t

  #print (" v_dt =", v_dt)
  u = v_dt[3:]
  #print (" u =", u)

  rd = RotationMatrix.buildFromThetaUVector(u)
  #print (" rd =", rd)


  theta = math.sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2])
  si = math.sin(theta)
  co = math.cos(theta)

  sinc = Math.sinc(theta)
  mcosc = Math.mcosc(co, theta)
  msinc = Math.msinc(si, theta)

  """ print (" theta =", theta)
  print (" si =", si)
  print (" co =", co)
  print (" sinc =", sinc)
  print (" mcosc =", mcosc)
  print (" msinc =", msinc) """

  dt = np.array([0.0, 0.0, 0.0])

  dt[0] = v_dt[0] * (sinc + u[0] * u[0] * msinc) + v_dt[1] * (u[0] * u[1] * msinc - u[2] * mcosc) +             v_dt[2] * (u[0] * u[2] * msinc + u[1] * mcosc);

  dt[1] = v_dt[0] * (u[0] * u[1] * msinc + u[2] * mcosc) + v_dt[1] * (sinc + u[1] * u[1] * msinc) +             v_dt[2] * (u[1] * u[2] * msinc - u[0] * mcosc);

  dt[2] = v_dt[0] * (u[0] * u[2] * msinc - u[1] * mcosc) + v_dt[1] * (u[1] * u[2] * msinc + u[0] * mcosc) +           v_dt[2] * (sinc + u[2] * u[2] * msinc);

  Delta = HomogeneousMatrix(np.eye(4))

  Delta.insertRotation(rd)

  Delta.matrix[0][3] = dt[0]
  Delta.matrix[1][3] = dt[1]
  Delta.matrix[2][3] = dt[2]

  return Delta
