import numpy as np
from sklearn import datasets, linear_model
import pandas as pd
import time
import csv
from learned_multiRegression import MultiRegression
from train_aidel import *
from learned_index import *



def select_data(step):
	X, Y = load_data('./data/lognormal.sorted.190M')
	#X, Y = load_timestamp('../data/timestamp.sorted.200M')
	#X, Y = load_id('../data/document-id.sorted.10M')
	a = []
	for i in xrange(0, len(X), step):
		a.append(X[i, 0])
		
	a = np.array(a).reshape(-1,1)
	b = np.arange(a.shape[0])
	
	print(a.shape, b.shape)
	return a, b


def load_timestamp(datafile):
	X = np.fromfile(datafile, dtype = np.int64).reshape(-1, 1)
	Y = np.arange(X.shape[0])
	print('read data: {0}, {1}'.format(X.shape, Y.shape))
	return X, Y

def load_id(datafile):
	df = pd.read_csv(datafile, header = None)
	
	X = np.array(df).reshape(-1, 1)
	Y = np.arange(len(X))
	print('read data: {0}, {1}'.format(X.shape, Y.shape))
	return X, Y

def train_10_lognormal():
	selectStep = 10
	#X, Y = load_data('./data/lognormal.sorted.190M')
	X, Y = load_data('./data/lognormal.select10.19M')
	dataset = "lognormal"
	
	
	d = 1
	w = 100000

	start_time = time.time()
	node = my_build_recursive(X, Y, w, d, 0)
	end_time = time.time()
	during_time = end_time - start_time
	print("number of nodes in model = " + str(Node.node_count))
	print('training time = {0}'.format(during_time))

	storepath = './data/learned_index/model_'+dataset+'/selectStep_'+str(selectStep)+'_secondStage_'+str(w/1000)+'K.txt'
	print(storepath)
	store_learnedindex(storepath, node)


def train_100_lognormal():
	selectStep = 100
	#X, Y = load_data('./data/lognormal.sorted.190M')
	X, Y = load_data('./data/lognormal.select100.1.9M')
	dataset = "lognormal"
	
	
	d = 1
	w = 10000

	start_time = time.time()
	node = my_build_recursive(X, Y, w, d, 0)
	end_time = time.time()
	during_time = end_time - start_time
	print("number of nodes in model = " + str(Node.node_count))
	print('training time = {0}'.format(during_time))

	storepath = './data/learned_index/model_'+dataset+'/selectStep_'+str(selectStep)+'_secondStage_'+str(w/1000)+'K.txt'
	print(storepath)
	store_learnedindex(storepath, node)


def train_timestamp():
	selectStep = 100
	X, Y = load_timestamp('./data/timestamp.sorted.200M')
	#X, Y = load_timestamp('./data/timestamp.select100.2M')
	dataset = "timestamp"
	
	
	d = 1
	w = 200000

	start_time = time.time()
	node = my_build_recursive(X, Y, w, d, 0)
	end_time = time.time()
	during_time = end_time - start_time
	print("number of nodes in model = " + str(Node.node_count))
	print('training time = {0}'.format(during_time))

	storepath = './data/learned_index/model_'+dataset+'/selectStep_'+str(selectStep)+'_secondStage_'+str(w/1000)+'K.txt'
	print(storepath)
	#store_learnedindex(storepath, node)


def train_document():
	selectStep = 100
	X, Y = load_timestamp('./data/document-id.sorted.10M')
	#X, Y = load_timestamp('./data/document-id.select100.100K')
	dataset = "document-id"
	
	
	d = 1
	w = 200000

	start_time = time.time()
	node = my_build_recursive(X, Y, w, d, 0)
	end_time = time.time()
	during_time = end_time - start_time
	print("number of nodes in model = " + str(Node.node_count))
	print('training time = {0}'.format(during_time))

	storepath = './data/learned_index/model_'+dataset+'/selectStep_'+str(selectStep)+'_secondStage_'+str(w/1000)+'K.txt'
	print(storepath)
	#store_learnedindex(storepath, node)



if __name__=='__main__':
	train_10_lognormal()
	train_100_lognormal()
	