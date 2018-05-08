import sys
from matplotlib import pyplot as plt
import numpy as np
from PIL import Image


with open("out.dat", 'r') as file:
    f = file.read().splitlines()

k, dims = f.pop(0).split(",")

colors = []

for i in range(int(k)):
    temp = f.pop(0)
    temp = temp.split(",")
    colors.append([np.uint8(float(x)) for x in temp])

# image = np.array(f).reshape(int(sys.argv[1]),int(sys.argv[2]))

f = list(map(int, f))

colored_image = np.array([colors[i] for i in f])

print(colored_image.reshape(int(sys.argv[1]), int(sys.argv[2]), 3)[0, :5, :])

im = Image.fromarray(colored_image.reshape(int(sys.argv[1]), int(sys.argv[2]), 3))
im.save("out.png")

#fig = plt.figure()
#plt.imshow(colored_image.reshape(int(sys.argv[1]), int(sys.argv[2]), 3))

"""
F = []
for line in f:
    F.append([np.uint8(i) for i in line.split(',')])

plt.figure()
plt.imshow(np.array(F).reshape(int(sys.argv[1]),int(sys.argv[2]),3))
plt.show()
"""
