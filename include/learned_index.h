#ifndef _LEARNED_INDEX_H_
#define _LEARNED_INDEX_H_

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <immintrin.h>
#include <stx/btree_map.h>


#include "levelbtree.h"
#include "search.h"
#include "readmodel.h"

using namespace std;

template<typename indexType, typename learnNodeType> class LearnedIndex {
	const int maxError_;
	learnNodeType root_node_;
	learnNodeType *models_;
	//stx::btree_map<indexType, indexType> btree_
	indexType *data_;
	int datasize_;
	int modelsize_;
	int errorModel_;
	indexType predict[3];

	stx::btree_map<indexType, indexType> deltaBuffer_;

public:
	explicit LearnedIndex(std::vector<learnNodeType> &tempmodels, std::vector<indexType>& data, int maxError) 
			:maxError_{maxError}{


		root_node_ = tempmodels[0];
		tempmodels.erase(tempmodels.begin()+0);

		models_ = process_models<learnNodeType>(tempmodels);
		modelsize_ = tempmodels.size();
		cout << "Learned_index -> read models: " << modelsize_ << endl;
		tempmodels.clear();

		// count how many model can't be used
		errorModel_ = 0;
		for (int i = 0; i < modelsize_; i++){
			if (models_[i].error > maxError_){
				errorModel_++;
			} 
		}
		cout << "Learned_index -> the maxError: " << maxError_ <<endl;
		cout << "Learned_index -> the model cann't be used(whose error is bigger than maxError): " << errorModel_ << endl;

		data_ = &data[0];
		datasize_=data.size();


		//insert the data into b+tree if the error > maxError
		for (int i = 0; i < datasize_; i++){
			indexType key = data_[i];
			double pred = root_node_.w * key + root_node_.b;
			int modelNumber = floor(((pred - root_node_.min)/(root_node_.max - root_node_.min))*(modelsize_-1));
			
			if (models_[modelNumber].error > maxError_){
				deltaBuffer_.insert2(key, key);
			}
		}

	}

	int find(indexType key){
		// the first level
		double pred = root_node_.w * key + root_node_.b;
		int modelNumber = floor(((pred - root_node_.min)/(root_node_.max - root_node_.min))*(modelsize_-1));	
		if (models_[modelNumber].error > maxError_){
			//typename stx::btree_map<indexType, indexType>::iterator it = btree_.find(key);
        	//return it==deltaBuffer_.end()? -1:it.data();
			//return btree_[key];
			return -1;	
		}
		
		pred = models_[modelNumber].w * key + models_[modelNumber].b;
		predict[0] = round(pred);
		predict[1]=predict[0]-maxError_;
		predict[2]=predict[0]+maxError_;
		if (predict[2] <= 0) return 0;
		if (predict[1] >= datasize_-1) return datasize_-1;
		if (predict[1] < 0) predict[1] = 0;
		if (predict[2] >= datasize_) predict[2] = datasize_-1;
		
		// binary search
		int i=binary_search_branchless(data_+predict[1], predict[2]-predict[1], key);
		//printf("predict[0]: %d,predict[1]: %d,predict[2]: %d,i: %d,\n", predict[0], predict[1],predict[2],i);
		return i+predict[1];

	}


	int find_avx(indexType key){
		// the first level
		double pred = root_node_.w * key + root_node_.b;
		int modelNumber = floor(((pred - root_node_.min)/(root_node_.max - root_node_.min))*(modelsize_-1));	
		if (models_[modelNumber].error > maxError_){
			//typename stx::btree_map<indexType, indexType>::iterator it = btree_.find(key);
			//return it==deltaBuffer_.end()? -1:it.data();
			return -1;
		}
		
		pred = models_[modelNumber].w * key + models_[modelNumber].b;
		predict[0] = round(pred);
		predict[1]=predict[0]-maxError_;
		predict[2]=predict[0]+maxError_;
		if (predict[2] <= 0) return 0;
		if (predict[1] >= datasize_-1) return datasize_-1;
		if (predict[1] < 0){
			predict[1] = 0;
			predict[2] = 2*maxError_;
		} 
		if (predict[2] >= datasize_) {
			predict[2] = datasize_-1;
			predict[1] = datasize_-1-2*maxError_;
		}

      	// seach in [pred-error, pred+error]
        if (data_[predict[1]] >= key) return predict[1];
		int i = linear_search_avx(data_ + predict[1]+1, 2*maxError_, key);
        //printf("i:%d, predict[0]: %d, predict[1]: %d, predict[2]: %d, %lf\n", i, predict[0], predict[1], predict[2], pred);
        return i+predict[1]+1;

	}


	int insert(indexType key){
		int pos = find_avx(key);
		if (pos!=-1 && key == data_[pos]) return -1;
		else deltaBuffer_.insert2(key,key);
		return 1;
	}

	indexType findInsert(indexType key){
		int pos = find_avx(key);
		if (pos != -1 && data_[pos]==key){
			return data_[pos];
		}
		typename stx::btree_map<indexType, indexType>::iterator it = deltaBuffer_.find(key);
        return it==deltaBuffer_.end()? 0:it.data();
	}



};

#endif
