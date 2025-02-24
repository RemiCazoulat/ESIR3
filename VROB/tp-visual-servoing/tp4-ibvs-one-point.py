# Copyright 2023 Eric Marchand, Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np
import math
import os
import sys
import argparse

import ExponentialMap
import Math
import Point
import RotationMatrix
import ThetaUVector
from HomogeneousMatrix import *
from PlotIbvs import *


def updateCameraPose(v, c_T_w, delta_t):
    """! Update camera pose after applying the velocity for delta_t seconds.
  @param[in] v The camera velocity to apply.
  @param[in] c_T_w The current position of the camera in the world frame.
  @param[in] delta_t The integration time in seconds.
  @return The new camera pose after applying the velocity for delta_t seconds.
  """
    c_T_c_delta_t = ExponentialMap.direct(v, delta_t)
    return c_T_c_delta_t.inverse() * c_T_w


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='The script corresponding to TP 4, IBVS on 1 point.')
    parser.add_argument('--plot-log-scale', action='store_true',
                        help='Plot error and norm of the error using a logarithmic scale')

    args, unknown_args = parser.parse_known_args()
    if unknown_args:
        print("The following args are not recognized and will not be used: %s" % unknown_args)
        sys.exit()

    # Position of the reference in the camera frame
    c_T_w = HomogeneousMatrix(np.eye(4))
    thetau = np.array([Math.deg2rad(0), Math.deg2rad(0), Math.deg2rad(0)])
    c_R_w = RotationMatrix.buildFromThetaUVector(thetau)
    c_T_w.insertRotation(c_R_w)
    c_T_w.matrix[0][3] = 0.0
    c_T_w.matrix[1][3] = 0.0
    c_T_w.matrix[2][3] = 1.0
    print("c_T_w = \n", c_T_w)

    # Desired position of the point is the center of the image
    xd = np.array([0.0, 0.0])

    # 3D point in the reference frame in homogeneous coordinates
    wX = np.array([0.5, 0.2, -0.5, 1.0])

    # Set the constant gain
    K = 0.1
    iter = 0
    norm_e = 1
    plot = 0
    # Control loop
    while norm_e > 0.001:
        print("------------------- iter ", iter)
        # BEGIN TO COMPLETE
        # Considered vars:
        #   e: error vector
        #   norm_e: norm of the error vector
        #   v: velocity to apply to the camera
        #   x: current visual feature vector
        #   xd: desired visual feature vector
        #   c_T_w: current position of the camera in the world frame
        #
        # ...
        # v
        cX = Point.changeFrame(c_T_w, wX)
        x = Point.project(cX)
        e = x - xd
        norm_e = np.linalg.norm(e)
        Lx = Point.interactionMatrix(x[0], x[1], cX[2])
        pLx = np.linalg.pinv(Lx)
        v = - K * np.matmul(pLx, e)
        # END TO COMPLETE

        if iter == 0:
            plot = PlotIbvs(e, norm_e, v, x, xd, c_T_w, args.plot_log_scale)
        else:
            plot.stack(e, norm_e, v, x, xd, c_T_w)

        # Compute the new position of the camera

        c_T_w = updateCameraPose(v, c_T_w, 0.040)
        print("x = ", x)
        print("e = ", e)
        print("Lx = ", Lx)
        print("norm e = ", norm_e)
        print("v = ", v)
        print("c_T_w = \n", c_T_w)

        # Increment iteration counter
        iter += 1

    print(f"\nConvergence achieved in {iter} iterations")

    # Display the servo behavior
    plot.display("results/fig-ibvs-one-point.png")

    print("Kill the figure to quit...")
