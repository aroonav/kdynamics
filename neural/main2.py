import numpy as np
import math
import csv

def dot(x, w):
	return np.dot(w, np.transpose(x))

def sigmoid(dot):
	s = np.empty(len(dot))
	for i in range(4):
		s[i] = (1/(1 + math.exp(-dot[i])))
	return s

def sigmoid_op(inp):
	return 1/(1 + math.exp(-inp))

def feed_forward(x, w1, w2):
	# print x.reshape((-1,1)).T.shape
	# print w1.shape
	# i_h1 = dot(x, w1)
	i_h1 = np.dot(w1, x.reshape((-1,1)))
	# print i_h1.shape
	o_h1 = sigmoid(i_h1)
	# print o_h1.shape
	# print w2.shape
	# print o_h1.reshape((1,-1)).shape
	# print w2.reshape((-1,1)).shape
	i_op = np.dot(o_h1, w2)
	# i_op = dot(o_h1, w2)
	o_op = sigmoid_op(i_op)
	# print "unc: ", o_op
	return [o_op, o_h1]

def calculate_error(output, target):
	return (target - output)**2

def d_layer_2(target,output):
	return (target - output)*(output)*(1 - output)

def d_layer_1(d, output, w2):
	e = np.dot(w2, d)
	return np.multiply(e, np.multiply(output, 1-output))

def backpropagation(output_op, hidden_op, w1, w2, learning, target):
	global x
	# print "target: ", target, output_op
	d = d_layer_2(target, output_op)
	d1 = d_layer_1(d, hidden_op, w2)
	# print d
	Y = np.dot(hidden_op, d)
	X = np.dot(d1.reshape((1,-1)).T, x.reshape((-1,1)).T)
	# print Y.shape, X.shape
	# print Y
	w2 = w2 + (learning * Y)
	w1 = w1 + (learning * X)
	return [w1, w2]


def initialize_input():
	file = open("/home/nox/Data/DSL-StrongPasswordData.csv")
	reader = csv.reader(file)
	reader.next()
	data = reader.next()
	for i in range(3):
		del data[0]
	data = map(float, data)
	return np.array(data)


DATA_PATH = "/home/nox/Data/DSL-StrongPasswordData.csv"

NO_INPUT_NODES = 31
NO_HIDDEN_NODES = 21 #number of input nodes = 2*P/3, where P is the number of input nodes
NO_OUTPUT_NODES = 1

WEIGHT_INIT = 0.1

target = 1.0
learning = 0.0001

# print x

#initialize the weights
w1 = np.array(np.full((NO_HIDDEN_NODES, NO_INPUT_NODES), WEIGHT_INIT))
w2 = np.array(np.full(NO_HIDDEN_NODES, WEIGHT_INIT))


# x = initialize_input()
# [output_op, hidden_op] = feed_forward(x, w1, w2)
# print "feed: ", output_op
# [w1, w2] = backpropagation(output_op, hidden_op, w1, w2, learning, target)


# file = open(DATA_PATH)
# reader = csv.reader(file)
# reader.next()
# for i in range(200):
# 	data = reader.next()
# 	for j in range(3):
# 		del data[0]
# 	data = map(float, data)
# 	x = np.array(data)
# 	#run below code for 500 epochs
# 	for k in range(500):
# 		[output_op, hidden_op] = feed_forward(x, w1, w2)
# 		# print output_op
# 		# error = calculate_error(output_op, target)
# 		[w1, w2] = backpropagation(output_op, hidden_op, w1, w2, learning, target)
# 	# print error
# for i in range(250):
# 	data = reader.next()
# 	popped = data.pop(0)
# 	for i in range(2):
# 		del data[0]
# 	data = map(float, data)
# 	x = np.array(data)
# 	[output_op, hidden_op] = feed_forward(x, w1, w2)
# 	print popped, ": ", output_op


# print w1
print w2

file = open("/home/nox/minor/inp")
reader = csv.reader(file, delimiter="\t")
for row in reader:
	# print w2
	# print row
	data = row
	popped = data.pop(0)
	for i in range(2):
		del data[0]
	data = map(float, data)
	x = np.array(data)
	[output_op, hidden_op] = feed_forward(x, w1, w2)
	# error = calculate_error(output_op, target)
	# [w1, w2] = backpropagation(output_op, hidden_op, w1, w2, learning, target)
	print popped, ": " , output_op