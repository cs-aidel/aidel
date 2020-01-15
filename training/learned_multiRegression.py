# -*- coding: utf-8 -*-

import math
import numpy as np
from sklearn import linear_model


class ModelNode:

	__slots__ = ['model', 'negError', 'fosError']

	def __init__(self, model, negError=0, fosError=0):
		self.model = model
		self.negError = negError
		self.fosError = fosError



class MultiRegression:
	
	__slots__ = ['_maxError', '_learningStep', '_learningRate', '_partision', '_models']

	def __init__(self, maxError, learningStep = 100000, learningRate = 0.1):
		self._maxError = maxError
		self._learningStep = learningStep
		self._learningRate = learningRate

		self._partision = []
		self._models = []


	def train(self, X, Y):

		print('training begin, length of training_data is: {0}'.format(X.shape[0]))


		start = 0
		end = self._learningStep
		while start != end:
			curX = X[start:end, :]
			curY = Y[start:end]

			model = linear_model.LinearRegression()
			model.fit(curX, curY)

			pred = model.predict(curX)
			diff = curY - pred
			negError = np.min(diff)
			fosError = np.max(diff)
			error = max(np.abs(negError), np.abs(fosError))
			modelnode = ModelNode(model, negError, fosError)

			if error == self._maxError:
				self._models.append(modelnode)
				self._partision.append(X[end-1, :])
				if len(self._partision) % 100 == 0:
					print('start = {0}, end = {1}, number = {2}, error = {3}'.format(X[start], X[end-1], end-start, error))

			elif error < self._maxError:
				if end == len(X):
					self._models.append(modelnode)
					self._partision.append(X[end-1, :])
					if len(self._partision) % 100 == 0:
						print('start = {0}, end = {1}, number = {2}, error = {3}'.format(X[start], X[end-1], end-start, error))
					break
				end += self._learningStep
				end = end if end < len(X) else len(X)
				continue

			else:
				offset = self._train(curX, curY, int(self._learningStep * self._learningRate))
				end = start + offset

			start = end
			end += self._learningStep
			end = end if end < len(X) else len(X)


	def _train(self, X, Y, step):
		
		while len(X) <= step:
			step = int(step * self._learningRate)
			if step <= 10:
				break;

		if len(X)<=step:
			
			model = linear_model.LinearRegression()
			model.fit(X, Y)

			pred = model.predict(X)
			diff = Y - pred
			negError = np.min(diff)
			fosError = np.max(diff)
			error = max(np.abs(negError), fosError)
			modelnode = ModelNode(model, negError, fosError)

			self._models.append(modelnode)
			self._partision.append(X[-1, :])
			print 'len(X) = {0}, step = {1}'.format(len(X), step)
			if len(self._partision) % 100 == 0:
				print('start = {0}, end = {1}, number = {2}, error = {3}'.format(X[0], X[-1], len(X), error))
			
			return len(X)
			

		else:
			end = len(X) - step
			while end > 0:
				curX = X[:end]
				curY = Y[:end]		

				model = linear_model.LinearRegression()
				model.fit(curX, curY)

				pred = model.predict(curX)
				diff = curY - pred
				negError = np.min(diff)
				fosError = np.max(diff)
				error = max(np.abs(negError), fosError)
				modelnode = ModelNode(model, negError, fosError)

				if error <= self._maxError:
					self._models.append(modelnode)
					self._partision.append(X[end-1, :])
					if len(self._partision) % 100 == 0:
						print('start = {0}, end = {1}, number = {2}, error = {3}'.format(X[0], X[end-1], end, error))
			
					return end
				end -= step

			end = self._train(X[:step, :], Y[:step], int(step * self._learningRate))
			return end


	def predict(self, key):
		
		lo = 0
		hi = len(self._partision)
		while lo < hi:
			mid = (lo+hi)//2
			mid_par = self._partision[mid]
			if mid_par[0] < key[0, 0]: lo = mid+1
			elif mid_par[0] > key[0, 0]: hi = mid
			else:
				if mid_par[1] < key[0, 1]: lo = mid+1
				else: hi = mid
		if lo >= len(self._partision):
			lo = len(self._partision) - 1
		
		y = self._models[lo].model.predict(key)[0]

		negy = y + self._models[lo].negError
		fosy = y + self._models[lo].fosError

		y = int(round(y))
		negy = int(math.floor(negy))
		fosy = int(math.ceil(fosy))
		return y, negy, fosy, lo


	def multi_predict(self, key, length):
		lo = 0
		hi = len(self._partision)
	
		while lo < hi:
			mid = (lo + hi)//2
			mid_par = self._partision[mid]
			for i in xrange(length):
				if mid_par[i] < key[0, i]:
					lo = mid + 1
					break
				else:
					if mid_par[i] == key[0, i] and i != length - 1: continue
					else:
						hi = mid
						break
		
		if lo >= len(self._partision):
			lo = len(self._partision) - 1		
		y = self._models[lo].model.predict(key)[0]
		negy = y + self._models[lo].negError
		fosy = y + self._models[lo].fosError
		y = int(round(y))
		negy = int(math.floor(negy))
		fosy = int(math.ceil(fosy))

		return y, negy, fosy, lo
		
		
