import sys

def format(value):
    if value < 0:
        return "%.9f" % value
    else:
        return "%.10f" % value


i = 0
print("Trimming data...")

with open('data/Activity recognition exp/Phones_accelerometer.csv', 'r') as fin, open('data/data_trim.csv', 'w') as fout:
    for line in fin:
        l = line.split(',')[1:6]
        if (any(c.isalpha() for c in l)):
            continue
        else:
            l = [format(float(v)) for v in l]
            fout.write(','.join(l)+'\n')
            i += 1
            #fout.write(line.replace('\t', ','))
print("Number of instances: {}".format(i))
