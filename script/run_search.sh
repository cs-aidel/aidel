#!/bin/bash

datapath="./data/lognormal.sorted.190M"
NUM_TRIALS=1


echo "============= Running aidel and the learned indexes ============"
./build/test_lognorm_190M $datapath ./data/lognormal_190M/model/error_64.txt 64 ./data/learned_index/model_lognormal/second_200000.txt 64

echo ""
echo "=================== Running search baselines ==================="
./build/range_search $datapath $NUM_TRIALS 48 48 48

echo ""
echo "===================== Running FAST baseline ===================="
./build/fast $datapath

