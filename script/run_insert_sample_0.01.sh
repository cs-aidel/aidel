#!/bin/bash


echo "=============== Running insert on sample 1% ===================="
for ((i=10; i<=100; i+=10)); do
	./build/insert_sample_0.01 ./data/lognormal.select100.1.9M ./data/lognormal_190M/model/selectStep_100_error_32.txt 32 ./data/learned_index/model_lognormal/selectStep_100_secondStage_10K.txt 32 $i
done
