import SimpleITK as sitk
import sys
import numpy as np


if len(sys.argv) > 1:
    path = sys.argv[1]
else:
    path = "data/image.jpg"
image = sitk.GetArrayFromImage(sitk.ReadImage(path))

im_flat = image.reshape(-1, 3)

print(image.shape)
print(im_flat[0, :])
print(im_flat.shape)
np.savetxt("data/image_data.csv", im_flat, delimiter=",", fmt="%03d")
