import sys
from matplotlib import pyplot as plt
import numpy as np

with open("out.dat", 'r') as file:
    f = file.read().splitlines()
print(f[-5:])
k, dims = f.pop(0).split(",")

colors = []

for i in range(int(k)):
    temp = f.pop(0)
    temp = temp.split(",")
    colors.append([np.float(x) for x in temp])

#image = np.array(f).reshape(int(sys.argv[1]),int(sys.argv[2]))

f = list(map(int, f))

colored_image =np.array([colors[i] for i in f])


plt.figure()
plt.imshow(colored_image.reshape(int(sys.argv[1]),int(sys.argv[2]),3))
plt.show()
