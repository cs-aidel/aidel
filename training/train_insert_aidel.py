#-*- encoding: utf-8 -*-

import time
import csv
import numpy as np
import pandas as pd
from learned_multiRegression import MultiRegression
from train_aidel import *



def select_data(step):
	X, Y = load_data('./data/lognormal.sorted.190M')
	#X, Y = load_timestamp('../data/timestamp.sorted.200M')
	#X, Y = load_id('../data/document-id.sorted.10M')
	a = []
	for i in xrange(0, len(X), step):
		a.append(X[i, 0])
		
	a = np.array(a).reshape(-1,1)
	b = np.arange(a.shape[0])
	
	print 'select data: {0}, {1}'.format(a.shape, b.shape)
	return a, b


def load_timestamp(datafile):
	X = np.fromfile(datafile, dtype = np.int64).reshape(-1, 1)
	Y = np.arange(X.shape[0])
	print 'read data: {0}, {1}'.format(X.shape, Y.shape)
	return X, Y

def load_id(datafile):
	df = pd.read_csv(datafile, header = None)
	
	X = np.array(df).reshape(-1, 1)
	Y = np.arange(len(X))
	print 'read data: {0}, {1}'.format(X.shape, Y.shape)
	return X, Y

def train_10_lognorm():
	selectStep = 10
	X, Y = load_data('./data/lognormal.select10.19M')
	#X, Y = load_data('./data/lognormal.sorted.190M')
	dataset = 'lognormal_190M'
	
	error = 31
	lmi = MultiRegression(maxError = error, learningStep = 10000)
	
	storepath = './data/'+dataset+'/model/selectStep_'+str(selectStep)+'_error_'+str(error+1)+'.txt'

	
	start_time = time.time()
	lmi.train(X, Y)
	end_time = time.time()
	during_time = end_time - start_time
	print "number of training model: {0}".format(len(lmi._models))
	print 'model number = {0}, maxError = {1}'.format(len(lmi._partision), lmi._maxError)
	print 'training time = {0}'.format(during_time)
	
	store_model(storepath, lmi)
	print(storepath)


def train_100_lognorm():
	selectStep = 100
	X, Y = load_data('./data/lognormal.select100.1.9M')
	#X, Y = load_data('./data/lognormal.sorted.190M')
	dataset = 'lognormal_190M'
	
	error = 31
	lmi = MultiRegression(maxError = error, learningStep = 1000)
	
	storepath = './data/'+dataset+'/model/selectStep_'+str(selectStep)+'_error_'+str(error+1)+'.txt'

	
	start_time = time.time()
	lmi.train(X, Y)
	end_time = time.time()
	during_time = end_time - start_time
	print "number of training model: {0}".format(len(lmi._models))
	print 'model number = {0}, maxError = {1}'.format(len(lmi._partision), lmi._maxError)
	print 'training time = {0}'.format(during_time)
	
	store_model(storepath, lmi)
	print(storepath)


if __name__ == '__main__':

	train_10_lognorm()
	train_100_lognorm()
	
