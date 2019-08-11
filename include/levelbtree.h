#ifndef _LEVELBTREE_H_
#define _LEVELBTREE_H_


#include "stx/btree_map.h"
#include "search.h"
#include "readdata.h"
#include "readmodel.h"

#include <iostream>
#include <climits>
#include <vector>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <numeric>
#include <random>
#include <utility>
#include <immintrin.h>
#include <fstream>
#include <time.h>
#include <sys/time.h>




class TwoLevelIndex {
  const int* data_;
  int k1_, k2_, k2stride_, page_size_;
  int* tables_;

 public:
  explicit TwoLevelIndex(std::vector<int>& data, int k2, int page_size)
      : k2_(k2), page_size_(page_size) {

    int hang = data.size() % page_size;
    if (hang > 0) {
      for (int i = 0; i < page_size - hang; i++) {
        data.push_back(INT_MAX);
      }
    }

    data_ = &data[0];

    k1_ = (int) std::ceil(data.size() / double(k2_ * page_size_));
    std::cerr << "2-level index top level size = " << k1_ << std::endl;
    tables_ = (int*) calloc((size_t) k1_ * (k2 + 1), sizeof(int));
    k2stride_ = k2 * page_size_;

    // populate second level index
    for (int i = 0; i < k1_ * k2_; i++) {
      tables_[k1_ + i] = ((i + 1) * page_size_ > data.size()) ? INT_MAX : data[(i + 1) * page_size_ - 1];
    }

    // populate top level index
    for (int i = 0; i < k1_; i++) {
      tables_[i] = tables_[k1_ + (i + 1) * k2 - 1];
    }
  }

  ~TwoLevelIndex() {
    free(tables_);
  }

  // assumes that key is <= max(data)
  int find(int key) {
    int i = binary_search_branchless(tables_, k1_, key);
    //int i = interpolation_search(tables_, k1_, key);
    //int i = linear_search_avx(tables_, k1_, key);
    //int j = binary_search_branchless(tables_ + k1_ + i * k2_, k2_, key);
    int j = linear_search_avx(tables_ + k1_ + i * k2_, k2_, key);
    int pos = i * k2stride_ + j * page_size_;
    int offset = linear_search_avx(data_ + pos, page_size_, key);
    /*printf("i:%d, j:%d, pos:%d, offset:%d\n", i,j,pos,offset);
    for (int a=0; a<5; a++){
        //printf("%d  ", tables_[k1_ + i * k2_ + a]);
        printf("%d  ", tables_[a]);
    }
    printf("\n");*/
    return pos + offset;
  }
};


class ThreeLevelIndex {
  const int* data_;
  int k1_, k2_, k3_, stride1_, stride2_, page_size_;
  int* tables_;
  int* table2_;
  int* table3_;

 public:
  explicit ThreeLevelIndex(std::vector<int>& data, int k2, int k3, int page_size)
      : k2_(k2), k3_(k3), page_size_(page_size) {

    int hang = data.size() % page_size;
    if (hang > 0) {
      for (int i = 0; i < page_size - hang; i++) {
        data.push_back(INT_MAX);
      }
    }

    data_ = &data[0];

    k1_ = (int) std::ceil(data.size() / double(k2_ * k3_ * page_size_));
    std::cerr << "3-level index top level size = " << k1_ << std::endl;
    tables_ = (int*) calloc((size_t) k1_ * (1 + k2_ * (1 + k3_)), sizeof(int));
    table2_ = tables_ + k1_;
    table3_ = table2_ + k1_ * k2_;

    stride1_ = k2_ * k3_ * page_size_;
    stride2_ = k3_ * page_size_;

    // populate third level index
    for (int i = 0; i < k1_ * k2_ * k3_; i++) {
      table3_[i] = ((i + 1) * page_size_ > data.size()) ? INT_MAX : data[(i + 1) * page_size_ - 1];
    }

    // populate second level index
    for (int i = 0; i < k1_ * k2_; i++) {
      table2_[i] = table3_[(i + 1) * k3_ - 1];
    }

    // populate top level index
    for (int i = 0; i < k1_; i++) {
      tables_[i] = table2_[(i + 1) * k2_ - 1];
    }
  }

  ~ThreeLevelIndex() {
    free(tables_);
  }

  // assumes that key is <= max(data)
  int find(int key) {
    int i = binary_search_branchless(tables_, k1_, key);
    //int i = interpolation_search(tables_, k1_, key);
    //int i = linear_search_avx(tables_, k1_, key);
    int j = linear_search_avx(table2_ + i * k2_, k2_, key);
    int k = linear_search_avx(table3_ + i * k2_ * k3_ + j * k3_, k3_, key);
    int pos = i * stride1_ + j * stride2_ + k * page_size_;
    int offset = linear_search_avx(data_ + pos, page_size_, key);
    return pos + offset;
  }
};


#endif