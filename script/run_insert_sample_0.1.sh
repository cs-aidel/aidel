#!/bin/bash


echo "=============== Running insert on sample 10% ===================="
for ((i=1; i<=10; i++)); do
	./build/insert_sample_0.1 ./data/lognormal.select10.19M ./data/lognormal_190M/model/selectStep_10_error_32.txt 32 ./data/learned_index/model_lognormal/selectStep_10_secondStage_100K.txt 32 $i
done

