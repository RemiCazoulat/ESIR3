# Copyright 2023 Eric Marchand Eric.Marchand@irisa.fr
#                Fabien Spindler, Fabien.Spindler@inria.fr

import matplotlib.pyplot as plt
import numpy as np
import os
import sys

# Use latex in plot legends and labels
plt.rc('text', usetex=plt.rcParamsDefault['text.usetex'])
plt.rc('text.latex', preamble=r'\usepackage{amsmath}')

class PlotPbvs:
  def __init__(self, e, norm_e, v, x, xd, c_T_w, cd_T_c, plot_log_scale):
    self.vector_e  = e
    self.vector_ne = norm_e
    self.vector_v  = v
    self.vector_x  = x
    self.vector_xd = xd
    self.vector_c_t_w = c_T_w.extractTranslation().flatten()
    self.vector_cd_t_c = cd_T_c.extractTranslation().flatten()
    self.plot_log_scale = plot_log_scale

  def stack(self, e, norm_e, v, x, xd, c_T_w, cd_T_c):
    self.vector_e  = np.vstack((self.vector_e, e))
    self.vector_ne = np.vstack((self.vector_ne, norm_e))
    self.vector_v  = np.vstack((self.vector_v, v))
    self.vector_x  = np.vstack((self.vector_x, x))
    self.vector_c_t_w = np.vstack((self.vector_c_t_w, c_T_w.extractTranslation().flatten()))
    self.vector_cd_t_c = np.vstack((self.vector_cd_t_c, cd_T_c.extractTranslation().flatten()))

  def display(self, fig_filename):
    plt.figure(figsize=(10,10))

    plot_e  = plt.subplot(2, 2, 1)
    plot_v  = plt.subplot(2, 2, 2)
    plot_ne = plt.subplot(2, 2, 3)
    plot_x  = plt.subplot(2, 2, 4)

    plot_e.set_title('error')
    plot_v.set_title('camera velocity')
    plot_x.set_title('point trajectory in the image plane')

    if self.plot_log_scale:
      plot_ne.set_title('log(norm error)')
      plot_ne.plot(np.log(self.vector_ne))
    else:
      plot_ne.set_title('norm error')
      plot_ne.plot(self.vector_ne)

    plot_ne.grid(True)
    plot_e.grid(True)
    plot_v.grid(True)
    plot_x.grid(True)

    plot_e.plot(self.vector_e)

    plot_e.legend(['${^{c^*}}tx_c$','${^{c^*}}ty_c$','${^{c^*}}tz_c$','$\Theta u_x$','$\Theta u_y$','$\Theta u_z$'])

    for i in range(self.vector_v.shape[1]): # Should be 6
      plot_v.plot(self.vector_v[:,i])
    plot_v.legend(['$v_x$','$v_y$','$v_z$','$\omega_x$','$\omega_y$','$\omega_z$'])

    for i in range(self.vector_x.shape[1] // 2): # Note: Use // to divide an int and return an int
      plot_x.plot(self.vector_x[:,2*i], self.vector_x[:,2*i+1])
    plot_x.legend(['$x_1$','$x_2$','$x_3$','$x_4$'])
    for i in range(self.vector_x.shape[1] // 2): # Note: Use // to divide an int and return an int
      plot_x.plot(self.vector_xd[2*i],  self.vector_xd[2*i+1],'o')

    # Create output folder it it doesn't exist
    output_folder = os.path.dirname(fig_filename)
    if not os.path.exists(output_folder):
      os.makedirs(output_folder)
      print("Create output folder: ", output_folder)

    print(f"Figure is saved in {fig_filename}")
    plt.savefig(fig_filename)

    # Plot 3D camera trajectory cd_t_c
    plot_cd_t_c = plt.figure().add_subplot(projection='3d')
    plot_cd_t_c.scatter(self.vector_cd_t_c[0, 0], self.vector_cd_t_c[0, 1], self.vector_cd_t_c[0, 2], marker='x', c='r', label='Initial position')
    plot_cd_t_c.scatter(0, 0, 0, marker='x', c='g', label='Desired position')
    # Hack to ensure that the scale is at minimum between -0.5 and 0.5 along X and Y axis
    min_s = np.min(self.vector_cd_t_c, axis=0)
    max_s = np.max(self.vector_cd_t_c, axis=0)
    for i in range(len(min_s)):
      if (max_s[i] - min_s[i]) < 1.:
        max_s[i] += 0.5
        min_s[i] -= 0.5
    plot_cd_t_c.axis(xmin=min_s[0], xmax=max_s[0])
    plot_cd_t_c.axis(ymin=min_s[1], ymax=max_s[1])

    line_debug = [self.vector_cd_t_c[0, 0], self.vector_cd_t_c[0, 1], self.vector_cd_t_c[0, 2]]
    line_debug = np.vstack((line_debug, [0, 0, 0]))
    plot_cd_t_c.plot(line_debug[:, 0], line_debug[:, 1], zs=line_debug[:, 2], label='Line debug')

    plot_cd_t_c.plot(self.vector_cd_t_c[:, 0], self.vector_cd_t_c[:, 1], zs=self.vector_cd_t_c[:, 2], label='Camera trajectory')
    plot_cd_t_c.set_title('c*_t_c trajectory')
    plot_cd_t_c.legend()

    filename = os.path.splitext(fig_filename)[0] + "-traj-cd_t_c.png"
    print(f"Figure is saved in {filename}")
    plt.savefig(filename)

    print("Kill the figure to quit...")
    plt.show()
