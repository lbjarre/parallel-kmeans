i = 0
with open('activity/Phones_accelerometer.csv') as fin, open('Phones_accelerometer.csv', 'w') as fout:
    for line in fin:
        i += 1
        l = line.split(',')[:6]
        fout.write(','.join(l)+'\n')
        #fout.write(line.replace('\t', ','))
print("Number of instances: {}".format(i))
