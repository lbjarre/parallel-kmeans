
print("Trimming data...")
i = 0
with open('data/Activity recognition exp/Phones_accelerometer.csv') as fin, open('data/data.csv', 'w') as fout:
    for line in fin:
        i += 1
        l = line.split(',')[:6]
        fout.write(','.join(l)+'\n')
        #fout.write(line.replace('\t', ','))
print("Number of instances: {}".format(i))
