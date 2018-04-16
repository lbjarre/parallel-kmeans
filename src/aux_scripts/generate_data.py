#!/usr/bin/env python3
"""
Generates K D-dimensional gaussian distributions and samples N/K number of datapoints from each distribution
"""
import os
import argparse
import numpy as np
from matplotlib import pyplot as plt


parser = argparse.ArgumentParser(
    description='''Generates K D-dimensional gaussian distributions and samples N/K number of datapoints from each distribution and saves them in two csv files containing the data and the correct labels.''',
    epilog='''Example: '''
    )

parser.add_argument('-N', help='Number of datapoints', action='store', type=int, default='128')
parser.add_argument('-K', help='Number of clusters', action='store', type=int, default='4')
parser.add_argument('-D', help='Number of dimensions', action='store', type=int, default='2')
parser.add_argument('--name', help='Path to save file', action='store', type=str, default='.')

args = parser.parse_args()

N = args.N
D = args.D
K = args.K
name = args.name

means = np.random.random_sample((K,D))
cov = np.eye(D) * 0.5
n_k = np.int(np.floor(N/K))

X, Y = [],[]
for k in range(K):
    X.extend(np.random.multivariate_normal(means[k,:], cov, n_k))
    Y.extend(np.ones(n_k)*k)

plt.figure()
for k in range(K):
    data = X[k*n_k:(k+1)*n_k,:]
    print(data)
    plt.scatter(X[k*n_k:(k+1)*n_k, :])
plt.show()

#numpy.savetxt(name+"_data.csv", X, delimiter=",")
