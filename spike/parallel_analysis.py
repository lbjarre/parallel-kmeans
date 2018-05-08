import numpy as np


M = []

with open("data/multi.csv", "r") as file:
    file.readline()
    for line in file:
        l = line.replace("\n", '').split(",")
        l = [int(x) for x in l]
        M.append(l)

M = np.array(M)
z = np.zeros((M.shape[0], M.shape[1]+1))

i = 0
temp = np.zeros((12, 3))

for row in M:
    z[i, :3] = row
    z[i, 3] = row[1] + row[2]
    i += 1
prev = 1
L1 = []
L2 = []
i = 0

temp = np.zeros((1, 4))
t = []

for row in z:
    if row[0] != prev:
        t = np.array(t)
        print(t)

        L1.append(np.mean(t, 0))
        L2.append(np.std(t, 0))
        t = []
        t.append(row)
    else:
        t.append(row)
    prev = row[0]

L1.append(np.mean(t, 0))
L2.append(np.var(t, 0))

L1 = np.array(L1)
L2 = np.array(L2)

L = np.hstack((L1, L2[:, 1:]))

np.savetxt("data/results_means_var.csv", L, delimiter=",", fmt="%-2f")

first_row = L[0, :]
temp = []
print(L)
for row in L:
    temp.append([np.divide(first_row[3], row[3]), np.divide(first_row[3], row[3]) / row[0]])

temp = np.array(temp)
temp = np.vstack(temp)
print(temp)

np.savetxt("data/parallel_speedup.csv", temp, delimiter=",", fmt="%-2f")
