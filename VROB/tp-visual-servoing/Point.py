# Copyright 2023 Eric Marchand Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np


def project(cX):
    """! Compute the projection of a 3D point in the image plane of a camera.
  @param[in] cX 3D coordinates (X, Y, Z) of a point in the camera frame as a 3-dim numpy array.
  @return The coordinates of the point in the image plane (x, y) after perspective projection as a 2-dim numpy array.
  """
    # BEGIN TO BE COMPLETED
    if cX[2] == 0:
        raise Exception("Z is equal to zero")
    x = cX / cX[2]
    # print("[DEBUG] > point > inter value : ", x, " > Z : ", cX[2])

    x = np.array([x[0], x[1]])
    # x = np.array([cX[0], cX[1]])

    # END TO BE COMPLETED
    # print("[DEBUG] > project > input value : ", cX, " > output value : ", x)

    return x


def changeFrame(a_T_b, bX):
    """! Compute the 3D coordinates of a point after frame transformation.
  @param[in] a_T_b The homogeneous matrix corresponding to the transformation from frame a to b as a 4-by-4 HomogeneousMatrix.
  @param[in] bX The 3D normalized coordinates (X, Y, Z, 1) of a point in frame b as a 4-dim numpy vector.
  @return The corresponding normalized coordinates of the point in frame a as a 4-dim numpy vector..
  """
    # compute the change of frame
    # BEGIN TO BE COMPLETED
    aX = a_T_b * bX
    # print("[DEBUG] > changeFrame > input value : ", bX, " > output value : ", aX)

    # END TO BE COMPLETED
    return aX


def interactionMatrix(x, y, Z):
    """! Compute the interaction matrix of a 2D point in the image plane of a camera.
  @param[in] x,y The coordinates (x, y) of a 2D point in the image plane.
  @param[in] Z The position along Z-camera axis of the corresponding 3D point in the camera frame.
  @return The interaction matrix of the 2D point.
  """
    # BEGIN TO BE COMPLETED
    # Here, Lx is size (6, 2)
    Lx = np.array([
      [-1/Z, 0, x/Z, x*y, -(1 + x*x), y],
      [0, -1/Z, y/Z, 1 + y*y, -x*y, - x]])
    # END TO BE COMPLETED
    return Lx
