import sys

def format(value):
    return "%.10f" % value


i = 0
print("Trimming data...")

with open('data/Activity recognition exp/Phones_accelerometer.csv', 'r') as fin, open('data/data.csv', 'w') as fout:
    for line in fin:
        i += 1
        l = line.split(',')[1:6]
        if (any(c.isalpha() for c in l)):
            i -= 1 
            continue
        l = [format(float(v)) for v in l]
        fout.write(','.join(l)+'\n')
        #fout.write(line.replace('\t', ','))
print("Number of instances: {}".format(i))
