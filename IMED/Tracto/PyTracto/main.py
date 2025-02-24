import scipy.io
import matplotlib.pyplot as plt
import numpy as np
import pyvista as pv
from constructMatrixOfMonomials import *
from constructSetOf321Polynomials import *
from lsqnoneg import *
from scipy.optimize import nnls
import time
import cvxpy as cp


mat = scipy.io.loadmat('Matlab-files/DTI_6dir.mat')  #Loading the mat
DTI_data = mat['DTI_Data']  #mat is a dict, we only want the (80, 96, 58, 7) matrix stored with the 'DTI_Data' key

b0 = DTI_data[:, :, :, 0]  #b0 is the anatomical structure


'''
# Select a specific slice
slice_index = 30
slice_to_display = b0[:, :, slice_index]

# Display the slice
plt.imshow(slice_to_display, cmap='gray')
plt.colorbar()
plt.title(f"Slice {slice_index} of b0 volume")
plt.axis('off')
plt.show()
b0_normalized = (b0 - np.min(b0)) / (np.max(b0) - np.min(b0))

# Adjust dimensions for cell-based data
grid = pv.ImageData(
    dimensions=(b0.shape[0] + 1,
                b0.shape[1] + 1,
                b0.shape[2] + 1),
    spacing=(1, 1, 1),
    origin=(0, 0, 0),
)

grid.cell_data["values"] = b0.flatten(order="F")


# Plot the grid
plotter = pv.Plotter()
plotter.add_volume(grid, cmap="gray", opacity="linear", scalar_bar_args={"title": "Intensity"})
plotter.show()
'''
