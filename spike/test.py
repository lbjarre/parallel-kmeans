import numpy as np


def factorial_count(x):
    n = 0
    while(np.abs(x)/(10**n) >= 10):
        n += 1
    return n


a = -10123233133
n = 12312308172301
b = 9876543210
q = 3

print(factorial_count(a))  # 11
print(factorial_count(b))  #
print(factorial_count(n))
print(factorial_count(q))
