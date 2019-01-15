#!/bin/python

import matplotlib.pyplot as plt
import numpy as np


# set line
x = np.linspace(0, 2, 100)


# GRAPH COORDS

# set coords for rate v const height
x_coords = [32,50,40,60,70,80,90,100,110,120]  # height
y_coords = [263,146,204,112,85,70,57,47,38,32] # rate

#variable height coords
variance = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
rate = [270, 223, 186, 156,132,113,99, 85, 77, 68, 59]


num_features = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
Rate = [196, 169, 152, 143, 138, 138, 133, 135, 136, 138, 137]

hund_n_feat = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
_rate = [202, 173, 146, 125, 110, 99, 92, 80, 78, 74,68]


#labels
xlabel = 'Number of Background Features Generated'   #'Image Height Variance Range (px)'    #'Image Height (px)'
ylabel = 'Production Rate (Hz)'

#plot points
plt.plot(num_features, Rate, 'go', label="Default probabilites")
plt.plot( hund_n_feat, _rate, 'bo', label="100% probabilities
")
#plt.plot(x_coords, y_coords, 'ro')

#axis labels
plt.xlabel(xlabel)
plt.ylabel(ylabel)

#plot title
plt.title("Production Rate vs. Number of Background Features")

#generate legend
plt.legend(loc='upper right')

#show the graph
plt.show()