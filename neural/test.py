import numpy as np

def action(a):
	for i in range(len(a)):
		a[i] = i

a = np.empty(6)
print a
action(a)
print a