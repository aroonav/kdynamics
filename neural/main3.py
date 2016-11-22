# # x = np.array([37.5, 31.0, 28.5, 28.8, 29.5, 30.2])
# w1 = np.array(np.full((4, 6), 0.1))
# w2 = np.array(np.full(4, 0.1))

# target = 1	#it should output 1 in case of a positive match
# learning = 0.0001  #the learning rate is set to 0.0001

# [output, hidden] = feed_forward()

# error = calculate_error(output, target)
# print "error: ", error

# #changing weights
# #the cost function
# d = -(target - output)*(output)*(1 - output)
# e = np.dot(w2, d)
# # print "e:",e 
# #temp
# d1 = np.multiply(e, np.multiply(output, 1-output))
# # print d1

# #the backward signal
# Y = np.dot(hidden, d)
# X = np.dot(d1.reshape((1,-1)).T, x.reshape((-1,1)).T)

# # print "Y:", Y
# # print "X:", X

# #update wieght2
# w2 = w2 - (learning * Y)
# w1 = w1 - (learning * X)