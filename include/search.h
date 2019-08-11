#ifndef _SEARCH_H_
#define _SEARCH_H_

#include <climits>
#include <vector>
#include <cmath>
#include <cstring>
#include <immintrin.h>


using namespace std;


#define SHUF(i0, i1, i2, i3) (i0 + i1*4 + i2*16 + i3*64)
#define FORCEINLINE __attribute__((always_inline)) inline

// power of 2 at most x, undefined for x == 0
FORCEINLINE uint32_t bsr(uint32_t x) {
  return 31 - __builtin_clz(x);
}
static void print_256(__m256i key){
    int32_t *p = (int*)&key;
    for (int i = 0; i < 8; i++){
        printf("%d  ", p[i]);
    }
    printf("\n");
}

static void print_256_long(__m256i key){
    int64_t *p = (long*)&key;
    for (int i = 0; i < 4; i++){
        printf("%ld  ", p[i]);
    }
    printf("\n");
}

static void print_128(__m128i key){
    int32_t *p = (int*)&key;
    for (int i = 0; i < 4; i++){
        printf("%d  ", p[i]);
    }
    printf("\n");
}

template<typename dataType> dataType binary_search_branchless(const dataType *arr, int n, dataType key) {
    intptr_t pos = -1;
    intptr_t logstep = bsr(n - 1);
    intptr_t step = intptr_t(1) << logstep;

    pos = (arr[pos + n - step] < key ? pos + n - step : pos);
    step >>= 1;

    while (step > 0) {
      pos = (arr[pos + step] < key ? pos + step : pos);
      step >>= 1;
    }
    pos += 1;

    return (dataType) (arr[pos] >= key ? pos : n);
}



static int linear_search_avx (const int *arr, int n, int key) {
    __m256i vkey = _mm256_set1_epi32(key);
    __m256i cnt = _mm256_setzero_si256();
    for (int i = 0; i < n; i += 16) {
      __m256i mask0 = _mm256_cmpgt_epi32(vkey, _mm256_load_si256((__m256i *)&arr[i+0]));
      __m256i mask1 = _mm256_cmpgt_epi32(vkey, _mm256_load_si256((__m256i *)&arr[i+8]));
      __m256i sum = _mm256_add_epi32(mask0, mask1);
      cnt = _mm256_sub_epi32(cnt, sum);
    }
    __m128i xcnt = _mm_add_epi32(_mm256_extracti128_si256(cnt, 1), _mm256_castsi256_si128(cnt));
    xcnt = _mm_add_epi32(xcnt, _mm_shuffle_epi32(xcnt, SHUF(2, 3, 0, 1)));
    xcnt = _mm_add_epi32(xcnt, _mm_shuffle_epi32(xcnt, SHUF(1, 0, 3, 2)));
    return _mm_cvtsi128_si32(xcnt);
}

static long linear_search_avx_long(const long *arr, int n, long key) {
    __m256i vkey = _mm256_set1_epi64x(key);
    __m256i cnt = _mm256_setzero_si256();
    for (int i = 0; i < n; i += 8) {
      __m256i mask0 = _mm256_cmpgt_epi64(vkey, _mm256_load_si256((__m256i *)&arr[i+0]));
      __m256i mask1 = _mm256_cmpgt_epi64(vkey, _mm256_load_si256((__m256i *)&arr[i+4]));
      __m256i sum = _mm256_add_epi64(mask0, mask1);
      cnt = _mm256_sub_epi64(cnt, sum);
    }
    __m128i xcnt = _mm_add_epi64(_mm256_extracti128_si256(cnt, 1), _mm256_castsi256_si128(cnt));
    xcnt = _mm_add_epi64(xcnt, _mm_shuffle_epi32(xcnt, SHUF(2, 3, 0, 1)));
    return _mm_cvtsi128_si64(xcnt);
}



static int linear_search_avx_8(const int *arr, int n, int key) {
    __m256i vkey = _mm256_set1_epi32(key);
    __m256i cnt = _mm256_setzero_si256();

    for (int i = 0; i < n; i += 8) {
        __m256i mask0 = _mm256_cmpgt_epi32(vkey, _mm256_load_si256((__m256i *)&arr[i+0]));
        cnt = _mm256_sub_epi32(cnt, mask0);
    }
    //print_256(cnt);

    __m128i xcnt = _mm_add_epi32(_mm256_extracti128_si256(cnt, 1), _mm256_castsi256_si128(cnt));
    xcnt = _mm_add_epi32(xcnt, _mm_shuffle_epi32(xcnt, SHUF(2, 3, 0, 1)));
    xcnt = _mm_add_epi32(xcnt, _mm_shuffle_epi32(xcnt, SHUF(1, 0, 3, 2)));

    //print_128(xcnt);
    //printf("%d\n", _mm_cvtsi128_si32(xcnt));

    return _mm_cvtsi128_si32(xcnt);
}


int binary_search_branchless_opt(const int *arr, int n, int key) {
    intptr_t pos = 0;
    intptr_t logstep = bsr(n - 1);
    intptr_t step = intptr_t(1) << logstep;

    pos = (arr[pos + step - 1] < key ? step : pos);
    step >>= 1;

    while (step > 32) {
      pos = (arr[pos + step - 1] < key ? pos + step : pos);
      step >>= 1;
    }
    //pos += 1;
  
    return (pos + linear_search_avx(&arr[pos], 64, key));
}



#endif