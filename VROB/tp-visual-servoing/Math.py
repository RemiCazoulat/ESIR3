# Copyright 2023 Eric Marchand Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np
import math
import sys

# -----------  Basic Math Stuff  ---------------
def sign(x):
  """! Return the sign of `x`.
  @param[in] x  Value to test.
  @return  When value is negative return -1. When value is positive return 1. When value is equal to zero return 0.
  """
  if abs(x) < sys.float_info.epsilon:
    return 0
  else:
    return -1 if x < 0 else 1

def sinc(x):
  """! Compute the sinus cardinal of `x`.
  @param[in] x  Value to consider.
  @return  Sinus cardinal of `x` as `sin(x)/x`.
  """
  if abs(x) < 1e-8:
    return 1.0
  else:
    return math.sin(x) / x

def mcosc(cosx, x):
  """! Compute `(1-cos(x))/x^2`.
  @param[in] cosx  Value of `cos(x)`.
  @param[in] x  Value of `x`.
  @return  Value of `(1-cos(x))/x^2`.
  """
  if abs(x) < 2.5e-4:
    return 0.5
  else:
    return (1 - cosx) / x / x

def msinc(sinx, x):
  """! Compute `(1-sinc(x))/x^2` where `sinc(x) = sin(x)/x`.
  @param[in] sinx  Value of `sin(x)`.
  @param[in] x  Value of `x`.
  @return  Value of `(1-sin(x))/x^2`.
  """
  if abs(x) < 2.5e-4:
    return 1 / 6.0
  else:
    return (1 - sinx / x) / x /x

def deg2rad(angle_deg):
  """! Convert an angle from degrees to radians.
  @param[in] angle_deg  Angle in degrees.
  @return  Corresponding value in radians.
  """
  return angle_deg * math.pi / 180.0

def min(a,b):
  """! Find the minimum between two values `a` and `b`.
  @param[in] a  First value to consider.
  @param[in] b  Second value to consider.
  @return  The minimum of the two numbers.
  """
  if a <= b:
    return a
  else:
    return b
