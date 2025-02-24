# Copyright 2023 Eric Marchand, Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import numpy as np
import math
import argparse
import sys
import os

import ExponentialMap
import Math
import Point
import RotationMatrix
import ThetaUVector
from HomogeneousMatrix import *
from PlotPbvs import *


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
    parser = argparse.ArgumentParser(description='The script corresponding to TP 4, PBVS on 4 points.')
    parser.add_argument('--initial-position', type=int, default=1, dest='initial_position',
                        help='Initial position selection (value 1, 2, 3 or 4)')
    parser.add_argument('--plot-log-scale', action='store_true',
                        help='Plot error and norm of the error using a logarithmic scale')

    args, unknown_args = parser.parse_known_args()
    if unknown_args:
        print("The following args are not recognized and will not be used: %s" % unknown_args)
        sys.exit()

    print("Use initial position %s" % args.initial_position)

    # Position of the reference in the camera frame
    c_T_w = HomogeneousMatrix(np.eye(4))

    if args.initial_position == 1:
        # - CASE 1
        thetau = np.array([0.0, 0.0, 0.0])
        c_R_w = RotationMatrix.buildFromThetaUVector(thetau)
        c_T_w.insertRotation(c_R_w)
        c_T_w.matrix[0][3] = 0.0
        c_T_w.matrix[1][3] = 0.0
        c_T_w.matrix[2][3] = 1.3
    elif args.initial_position == 2:
        # - CASE 2
        thetau = np.array([Math.deg2rad(10), Math.deg2rad(20), Math.deg2rad(30)])
        c_R_w = RotationMatrix.buildFromThetaUVector(thetau)
        c_T_w.insertRotation(c_R_w)
        c_T_w.matrix[0][3] = -0.2
        c_T_w.matrix[1][3] = -0.1
        c_T_w.matrix[2][3] = 1.3
    elif args.initial_position == 3:
        # - CASE 3 : 90 rotation along Z axis
        thetau = np.array([0.0, 0.0, Math.deg2rad(90)])
        c_R_w = RotationMatrix.buildFromThetaUVector(thetau)
        c_T_w.insertRotation(c_R_w)
        c_T_w.matrix[0][3] = 0.0
        c_T_w.matrix[1][3] = 0.0
        c_T_w.matrix[2][3] = 1.0
    elif args.initial_position == 4:
        # - CASE 3 : 180 rotation along Z axis
        thetau = np.array([0.0, 0.0, Math.deg2rad(180)])
        c_R_w = RotationMatrix.buildFromThetaUVector(thetau)
        c_T_w.insertRotation(c_R_w)
        c_T_w.matrix[0][3] = 0.0
        c_T_w.matrix[1][3] = 0.0
        c_T_w.matrix[2][3] = 1.0
    elif args.initial_position == 5:
        # - CASE 5 : Test
        thetau = np.array([Math.deg2rad(25), Math.deg2rad(30), Math.deg2rad(-50)])
        c_R_w = RotationMatrix.buildFromThetaUVector(thetau)
        c_T_w.insertRotation(c_R_w)
        c_T_w.matrix[0][3] = -0.1
        c_T_w.matrix[1][3] = 0.1
        c_T_w.matrix[2][3] = 1.5
    else:
        raise ValueError(f"Wrong initial position value. Values are 1, 2, 3 or 4")

    print("c_T_w = \n", c_T_w)

    # Position of the desired camera in the world reference frame
    cd_T_w = HomogeneousMatrix(np.eye(4))
    thetau = np.array([0, 0, 0])
    cd_R_w = RotationMatrix.buildFromThetaUVector(thetau)
    cd_T_w.insertRotation(cd_R_w)
    cd_T_w.matrix[0][3] = 0.0
    cd_T_w.matrix[1][3] = 0.0
    cd_T_w.matrix[2][3] = 1
    print("cd_T_w = \n", cd_T_w)

    # 3D point in the reference frame in homogeneous coordinates (just for visualization)
    wX = []
    wX.append(np.array([-0.1, 0.1, 0.0, 1]))
    wX.append(np.array([0.1, 0.1, 0.0, 1]))
    wX.append(np.array([0.1, -0.1, 0.0, 1]))
    wX.append(np.array([-0.1, -0.1, 0.0, 1]))

    print("wX =", wX)

    # 3D point in the camera frame in homogeneous coordinates
    # Creation of the x, xd and Z arrays

    # BEGIN TO COMPLETE
    x = np.zeros((2 * 4))
    xd = np.zeros((2 * 4))
    # END TO COMPLETE

    # Just for the point trajectory visualization in the 2D plane
    # BEGIN TO COMPLETE
    for i in range(len(wX)):
        cXd = Point.changeFrame(cd_T_w, wX[i])
        xd[2 * i:2 * i + 2] = Point.project(cXd)    # END TO COMPLETE

    # Set the constant gain
    K = 0.1
    norm_e = 1
    iter = 0

    # Control loop
    while norm_e > 0.0001:
        print("------------------- iter ", iter)
        # BEGIN TO COMPLETE
        # Considered vars:
        #   e: visual features error vector
        #   norm_e: norm of the visual features error vector
        #   v: velocity to apply to the camera
        #   x: current 2D position of the points in the image plane only for visualization
        #   xd: desired 2D position of the points in the image plane only for visualization
        #   c_T_w: current position of the camera in the world frame
        #   cd_T_w: desired position of the camera in the world frame

        # Just for the point trajectory visualization in the 2D plane
        for i in range(len(wX)):
            cX = Point.changeFrame(c_T_w, wX[i])
            x[2*i:2*i+2] = Point.project(cX)

        cd_T_c = cd_T_w * c_T_w.inverse()
        thetaU = cd_T_c.extractThetaU()
        cd_t_c = cd_T_c.extractTranslation()
        c_R_cd = cd_T_c.inverse().extractRotation()

        tmp = c_R_cd @ cd_t_c

        tmp = np.concatenate((tmp, thetaU), axis=None)

        tmp = np.array(tmp)
        v = -K * tmp
        # printing

        e = x - xd
        norm_e = np.linalg.norm(e)
        # END TO COMPLETE

        if iter == 0:
            plot = PlotPbvs(e, norm_e, v, x, xd, c_T_w, cd_T_c, args.plot_log_scale)
        else:
            plot.stack(e, norm_e, v, x, xd, c_T_w, cd_T_c)

        # Compute the new position of the camera
        c_T_w = updateCameraPose(v, c_T_w, 0.040)

        print("e = ", e)
        print("norm e = ", norm_e)
        print("v =", v)
        print("c_T_w = \n", c_T_w)
        print("cd_T_c = \n", cd_T_c)

        iter += 1

    print(f"\nConvergence achieved in {iter} iterations")

    # Display the servo behavior
    plot.display("results/fig-pbvs-four-points-initial-position-" + str(args.initial_position) + ".png")

    print("Kill the figure to quit...")
