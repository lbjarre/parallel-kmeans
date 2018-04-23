import urllib.request as urllib2
import zipfile
import os

url = "https://archive.ics.uci.edu/ml/machine-learning-databases/00344/Activity%20recognition%20exp.zip"

path = "data/data.zip"

if not os.path.isfile(path):

    print("Downloading 771 mb")

    urllib2.urlretrieve(url, path)

    print("Download complete")
    print("Extracting")

    zip_ref = zipfile.ZipFile(path, 'r')
    zip_ref.extractall("data")
    zip_ref.close()

else:
    
    print("Data already downloaded")

