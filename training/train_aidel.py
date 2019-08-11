#-*- encoding: utf-8 -*-

import time
import csv
import numpy as np
from learned_multiRegression import MultiRegression


DIR             = './data/'
DATAFILE        = DIR+'lognormal.sorted.190M'
STORE 			= DIR+'/lognormal_190M/model/error_64.txt'



def load_data(datafile):
	X = np.fromfile(datafile, dtype = np.intc).reshape(-1, 1)
	Y = np.arange(X.shape[0])
	print('read data: {0}, {1}'.format(X.shape, Y.shape))
	return X, Y


def store_model(datafile, lmi):

	outputfile = open(datafile,'w')
	for i in xrange(len(lmi._models)):
		model = lmi._models[i]
		index = lmi._partision[i][0]
		writedata = '{0},{1},{2},{3},{4}\n'.format(str(index),
			str(model.model.coef_[0]),str(model.model.intercept_),str(model.negError),str(model.fosError))
		#print writedata
		outputfile.write(writedata)

	outputfile.close()




if __name__ == '__main__':
	X, Y = load_data(DATAFILE)
	
	lmi = MultiRegression(maxError = 64, learningStep = 10000)

	start_time = time.time()
	lmi.train(X, Y)
	end_time = time.time()
	during_time = end_time - start_time
	print("number of training model: {0}".format(len(lmi._models)))
	print('model number = {0}, maxError = {1}'.format(len(lmi._partision), lmi._maxError))
	print('training time = {0}'.format(during_time))

	
	store_model(STORE, lmi)
	print(STORE)
	