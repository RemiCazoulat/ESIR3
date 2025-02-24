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
    parser = argparse.ArgumentParser(description='The script corresponding to TP 4, IBVS on 4 points.')
    parser.add_argument('--initial-position', type=int, default=1, dest='initial_position',
                        help='Initial position selection (value 1, 2, 3 or 4)')
    parser.add_argument('--interaction', type=str, default="current", dest='interaction_matrix_type',
                        help='Interaction matrix type (value \"current\" or \"desired\")')
    parser.add_argument('--move-target', action='store_true', help='Apply a motion to the target')
    parser.add_argument('--enable-integrator', action='store_true',
                        help='Enable target tracking by adding an integrator')
    parser.add_argument('--mu', type=float, default=0.001, dest='mu', help='Integrator gain (default: 0.001)')
    parser.add_argument('--plot-log-scale', action='store_true',
                        help='Plot error and norm of the error using a logarithmic scale')

    args, unknown_args = parser.parse_known_args()
    if unknown_args:
        print("The following args are not recognized and will not be used: %s" % unknown_args)
        sys.exit()

    print(f"Use initial position {args.initial_position}")
    print(f"Integrator enabled: {args.enable_integrator}")

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

    # 3D point in the reference frame in homogeneous coordinates
    wX = [
        np.array([-0.1,  0.1, 0.0, 1]),
        np.array([ 0.1,  0.1, 0.0, 1]),
        np.array([ 0.1, -0.1, 0.0, 1]),
        np.array([-0.1, -0.1, 0.0, 1])
    ]

    print("wX =", wX)

    # BEGIN TO COMPLETE
    # Creation of the x, xd, Z and Zd arrays for each 2D point
    x = np.zeros(2 * 4)  # Initialize current visual feature
    Z = np.zeros(4)  # Initialize current Z
    xd = np.zeros(2 * 4)  # Initialize desired visual feature
    Zd = np.zeros(4)  # Initialize desired Z
    e = np.zeros(2 * 4)  # Initialize error vector
    Lx = np.zeros((2 * 4, 6))  # Initialize interaction matrix
    # END TO COMPLETE

    # For each point
    # - compute the desired visual features and update xd
    # - compute the coordinates of the desired 3D point along Z axis and update Zd
    # BEGIN TO COMPLETE
    for i in range(len(wX)):
        cXd = Point.changeFrame(cd_T_w, wX[i])
        xd[2 * i:2 * i + 2] = Point.project(cXd)
        Zd[i] = cXd[2]
    # END TO COMPLETE

    print("------------------")

    print("xd =", xd)
    print("Zd =", Zd)

    # Set the constant gain
    K = 0.1
    iter = 0
    norm_e = 1

    # parameters for moving target
    L = args.mu
    error_sum = 0

    # Control loop
    while norm_e > 0.0001:
        print("------------------- iter ", iter)
        # Considered vars:
        #   e: error vector
        #   norm_e: norm of the error vector
        #   v: velocity to apply to the camera
        #   x: current visual feature vector
        #   xd: desired visual feature vector
        #   c_T_w: current position of the camera in the world frame

        # Apply a velocity to the target when option is enabled
        if args.move_target:
            for i in range(len(wX)):
                wX[i][0] += 0.001  # X coordinate

        if args.interaction_matrix_type == "current":
            # BEGIN TO COMPLETE
            print("Use Lx|x=x(t)")
            # Update current 2D position of the points in the image plane in x, coordinate along Z-axis in Z and the
            # interaction matrix lines in Lx for each point
            for i in range(len(wX)):
                cX = Point.changeFrame(c_T_w, wX[i])
                x[2 * i:2 * i + 2] = Point.project(cX)
                Z[i] = cX[2]
                Lx[2 * i:2 * i + 2, 0:6] = Point.interactionMatrix(x[2 * i], x[2 * i + 1], Z[i])
            # END TO COMPLETE
        elif args.interaction_matrix_type == "desired":
            # BEGIN TO COMPLETE
            print("Use Lx|x=x*")
            # Update current 2D position of the points in the image plane in x, and the interaction matrix lines in
            # Lx for each point for i in range(len(wX)): x[2*i:2*i+2] = ... Lx[2*i:2*i+2, 0:6] = ... END TO COMPLETE
            for i in range(len(wX)):
                Lx[2 * i:2 * i + 2, 0:6] = Point.interactionMatrix(xd[2 * i], xd[2 * i + 1], Zd[i])

        else:
            raise ValueError(f"Wrong interaction matrix type. Values are \"current\" or \"desired\"")

        # BEGIN TO COMPLETE
        e = x - xd
        norm_e = np.linalg.norm(e)
        pLx = np.linalg.pinv(Lx)

        if args.enable_integrator:
            # test
            error_sum += e
            errors = - K * e - L * error_sum
            v = np.matmul(pLx, errors)

        else:
            v = - K * np.matmul(pLx, e)
        # END TO COMPLETE

        if iter == 0:
            plot = PlotIbvs(e, norm_e, v, x, xd, c_T_w, args.plot_log_scale)
        else:
            plot.stack(e, norm_e, v, x, xd, c_T_w)

        # Compute the new position of the camera
        c_T_w = updateCameraPose(v, c_T_w, 0.040)

        print("x = ", x)
        print("xd = ", xd)
        print("e = ", e)
        print("norm e = ", norm_e)
        print("Lx = ", Lx)
        print("v = ", v)
        print("c_T_w = \n", c_T_w)

        # Increment iteration counter
        iter += 1

    print(f"\nConvergence achieved in {iter} iterations")

    # Display the servo behavior
    plot.display("results/fig-ibvs-four-points-initial-position-" + str(args.initial_position) + ".png")

    print("Kill the figure to quit...")
