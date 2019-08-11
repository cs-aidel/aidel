#ifndef _NEWAIDEL_H_
#define _NEWAIDEL_H_

#include <iostream>
#include <utility>
#include <immintrin.h>

#include "insert_aidel.h"
#include "search.h"
#include "levelbtree.h"
#include "readmodel.h"


using namespace std;

template<typename indexType, typename nodeType, typename iRType, typename iNType>
class AIDEL {
public:
	const nodeType * models_;
    int modelsize_;

    const indexType* data_;
	int datasize_;

    const int maxError_;
	int predict[3];

    const indexType* index_;
    indexType* tables_;
    int k1_, page_size_, padding_;

    iRType *insert_;
    iRType *insert_level_;

    stx::btree_map<indexType, indexType> deltaBuffer_;

public:
	explicit AIDEL(std::vector<nodeType>& models, std::vector<indexType>& index, std::vector<indexType>& data, const int maxError)
			: maxError_(maxError) {

        // construct the mdoels
		models_ = &models[0];
		modelsize_ = models.size();
		printf("aidel -> read models: %d\n", modelsize_);

        // construct the data
        data_ = &data[0];
        datasize_=data.size();
        printf("aidel -> read data: %d\n", datasize_);


        // construct the index for models
        index_ = &index[0];
        page_size_ = 64;
        padding_ = index.size();
        printf("aidel -> padding models: %d\n", padding_);

        k1_ = (int) std::ceil(index.size() / double(page_size_));
        std::cerr << "aidel -> 1-level index top level size = " << k1_ << std::endl;
        tables_ = (indexType*) calloc((size_t) k1_ , sizeof(indexType));
        for (int i = 0; i < k1_; i++) {
            tables_[i] = ((i + 1) * page_size_ > index.size()) ? INT_MAX : index[(i + 1) * page_size_ - 1];
        }

        // initial insert node
        insert_ = process_insertRoot<iRType, indexType>(data);
        insert_level_ = process_insertRoot<iRType, indexType>(data);

	}

	int find(indexType key){
        int i = binary_search_branchless(tables_, k1_, key);
        int offset = linear_search_avx(index_ + i * page_size_, page_size_, key);
        //printf("i: %d, offset: %d\n", i, offset);
        i = i*page_size_ + offset;
        
		if(i>=modelsize_) i=modelsize_-1;
		double pred=models_[i].w*key + models_[i].b;
		predict[0] = round(pred);
        predict[1]=predict[0]-maxError_;
        predict[2]=predict[0]+maxError_;
        //printf("i:%d, predict[0]: %d, predict[1]: %d, predict[2]: %d, %lf\n", i, predict[0], predict[1], predict[2], pred);
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
		i = linear_search_avx(data_ + predict[1]+1, 2*maxError_, key);
        //printf("i:%d, predict[0]: %d, predict[1]: %d, predict[2]: %d, %lf\n", i, predict[0], predict[1], predict[2], pred);
        return i+predict[1]+1;
	}

    int find2(indexType key){
        int pos = find(key);
        //printf("pos: %d, data_[i-1]:%d, data_[i]:%d, data_[i]:%d\n", pos,data_[pos-1], data_[pos], data_[pos+1]);
        pos = data_[pos]==key? data_[pos]:-1;
        //printf("pos: %d, key: %d\n", pos, key);
        return pos;
    }


    // ======================insert with sorted list======================
    int insert(indexType key) {
        int pos = find(key);
        if (key == data_[pos]) return -1;

        // insert into the array 
        iRType* ir = &(insert_[pos]);
        return insertKey<iRType, int>(ir, key);
    
    }

    indexType findInsert(indexType key){
        int pos = find(key);
        if (key == data_[pos]) return data_[pos];

        iRType* ir = &(insert_[pos]);
        return findKey<iRType, int>(ir, key);
    }

    
    // =========================insert with deltaBuffer=========================
    int insert2(indexType key) {
        int pos = find(key);
        if (key == data_[pos]) return -1;
        deltaBuffer_.insert2(key,key);
        //printf("pos: %d, data_[i-1]:%d, data_[i]:%d, data_[i]:%d\n", pos,data_[pos-1], data_[pos], data_[pos+1]);
        return 1;
    
    }

    indexType findInsert2(indexType key){
        int pos = find(key);
        if (key == data_[pos]) return data_[pos];
        typename stx::btree_map<indexType, indexType>::iterator it = deltaBuffer_.find(key);
        return it==deltaBuffer_.end()? 0:it.data();
    }

    // =============== insert with level-bins=======================
    int insert_level(indexType key) {
        int pos = find(key);
        if (key == data_[pos]) return -1;

        // insert into the array 
        iRType* ir = &(insert_level_[pos]);
        return insertKey_level<iRType, int>(ir, key);
    
    }

    indexType findInsert_level(indexType key){
        int pos = find(key);
        if (key == data_[pos]) return data_[pos];


        iRType* ir = &(insert_level_[pos]);
        return findKey_level<iRType, int>(ir, key);
    }

};

#endif