i = 0
with open('Skin_NonSkin.txt') as fin, open('Skin_NonSkin.csv', 'w') as fout:
    for line in fin:
        i += 1
        fout.write(line.replace('\t', ',')[:-3]+"\n")
print("Number of instances: {}".format(i))
