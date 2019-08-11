import numpy as np
from sklearn import datasets, linear_model
import pandas as pd
import time
import csv
from learned_multiRegression import MultiRegression
from train_aidel import *


class Node:
    node_count = 0
    def __init__(self):
        Node.node_count += 1
        self.m = 0
        self.b = 0
        self.children = []
        self.max = 0
        self.min = 0

        self.error = 0

def my_build_recursive(X, Y, w, d, current_d):
    X = X.reshape(len(X),1)
    Y = Y.reshape(len(Y),1)

    reg = linear_model.LinearRegression()
    reg.fit(X, Y)
    node = Node()   
    
    node.m = reg.coef_[0][0] # slope
    node.b = reg.intercept_[0] # intercept
    pred = X * node.m + node.b # gen prediction array using mx+b formula for Y values
    node.min = np.min(pred)
    node.max = np.max(pred)

    diff = Y - pred
    negError = np.min(diff)
    fosError = np.max(diff)
    error = max(np.abs(negError), np.abs(fosError))
    node.error = error
    
    if Node.node_count%10000==0:
        print(Node.node_count,node.m,node.b,node.error)

    # if desired depth not reached
    if current_d != d and node.max - node.min > 0:
        bins = np.floor(((pred - node.min) / (node.max - node.min)) * (w-1))
        bins = bins.astype(int)
        
        subX = []
        subY = []
        for i in xrange(w):
            subX.append([])
            subY.append([])
        
        for i in xrange(len(bins)):
            subX[bins[i,0]].append(X[i,0])
            subY[bins[i,0]].append(Y[i,0])
            if i % 10000000 == 0:
                print i

        for i in xrange(w):
            if len(subX[i]) > 0:
                Xwi = np.array(subX[i]).reshape(-1,1)
                Ywi = np.array(subY[i]).reshape(-1,1)
                child_node = my_build_recursive(Xwi, Ywi, w, d, current_d + 1)
                node.children.append(child_node)
            else:
                node.children.append(None)

    return node


def build_recursive(X, Y, w, d, current_d):
    X = X.reshape(len(X),1)
    Y = Y.reshape(len(Y),1)
    
    reg = linear_model.LinearRegression()
    reg.fit(X, Y)
    node = Node()   
    
    node.m = reg.coef_[0][0] # slope
    node.b = reg.intercept_[0] # intercept
    pred = X * node.m + node.b # gen prediction array using mx+b formula for Y values
    node.min = np.min(pred)
    node.max = np.max(pred)

    diff = Y - pred
    negError = np.min(diff)
    fosError = np.max(diff)
    error = max(np.abs(negError), np.abs(fosError))
    node.error = error
    
    print Node.node_count,node.m,node.b,node.error

    # if desired depth not reached
    if current_d != d and node.max - node.min > 0:
        bins = np.floor(((pred - node.min) / (node.max - node.min)) * (w-1))
        bins = bins.astype(int)
        for wi in range(w):
            mask = bins == wi # list of bins which equal wi
            Xwi = X[mask] # list of values that fall into the bin
            Ywi = Y[mask] 
            
            if len(Xwi) > 0 and len(Ywi) > 0:
                child_node = build_recursive(Xwi, Ywi, w, d, current_d + 1)
                node.children.append(child_node)
            else:
                node.children.append(None)
            
    return node    
    
def predict_recursive(x, w, d, node):
    pred = x * node.m + node.b 
    
    if len(node.children) > 0 and node.max - node.min > 0:
        bin = np.floor(((pred - node.min) / (node.max - node.min)) * (w-1))
        bin = bin.astype(int)
        if bin >= 0 and len(node.children) > bin and node.children[bin] != None:
            pred = predict_recursive(x, w, d, node.children[bin])
        
        
    return pred


def store_learnedindex(datafile, node):

    outputfile = open(datafile,'w')
    writedata = '{0},{1},{2},{3},{4}\n'.format(str(node.m),str(node.b),
            str(node.min),str(node.max),str(node.error))
    outputfile.write(writedata)
    for i in xrange(len(node.children)):
        model = node.children[i]
        if model==None:
            writedata = '{0},{1},{2},{3},{4}\n'.format(str(0),str(0),str(0),str(0),str(100000))
        else:
            writedata = '{0},{1},{2},{3},{4}\n'.format(str(model.m),str(model.b),
            str(model.min),str(model.max),str(model.error))
        #print writedata
        outputfile.write(writedata)

    outputfile.close()

def load_timestamp(datafile):
    X = np.fromfile(datafile, dtype = np.int64).reshape(-1, 1)
    Y = np.arange(X.shape[0])
    print 'read data: {0}, {1}'.format(X.shape, Y.shape)
    return X, Y

def load_id(datafile):
    df = pd.read_csv(datafile, header = None)
    
    X = np.array(df).reshape(-1, 1)
    Y = np.arange(len(X))
    print ('read data: {0}, {1}'.format(X.shape, Y.shape))
    return X, Y

            
if __name__ == "__main__": 

    # load csv and columns
    X, Y = load_data('./data/lognormal.sorted.190M')
    

    
    # build learned index model
    d = 1
    w = 200000
    dataset = 'lognormal'
    storepath = './data/learned_index/model_'+dataset+'/second_'+str(w)+'.txt'
    
    start_time = time.time()
    node = my_build_recursive(X, Y, w, d, 0)
    end_time = time.time()
    during_time = end_time - start_time
    print("number of nodes in model = " + str(Node.node_count))
    print('training time = {0}'.format(during_time))

    store_learnedindex(storepath, node)
    print(storepath)
    
    
    