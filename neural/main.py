import theano
import theano.tensor as T
import theano.tensor.nnet as nnet
import numpy as np

# x = T.dvector()
# y = T.dscalar()

# def activation(x,w):
# 	bias = np.array([1], dtype = theano.config.floatX)
# 	print bias
# 	print x
# 	print w
# 	new_x = T.concatenate([x, bias])
# 	f1 = theano.function([x, bias], new_x)
# 	print new_x
# 	inp = T.dot(w.T, new_x)
# 	# print o1
# 	# act = nnet.sigmoid(inp)
# 	act = 1/(1 + T.exp(-inp))
# 	return act

# x = np.array([37.5, 31.0, 28.5, 28.8, 29.5, 30.2], dtype=theano.config.floatX)

# m = np.array(np.full((4, 6), 0.1), dtype=theano.config.floatX)

# c = activation(x, m)
# print c

x = T.dvector('x')
w = T.dmatrix('w')

w = np.array(np.full((4, 6), 0.1), dtype=theano.config.floatX)

x = np.array([37.5, 31.0, 28.5, 28.8, 29.5, 30.2], dtype=theano.config.floatX)
inp = T.dot(w.T, x)

f1 = theano.function([x,w], inp)