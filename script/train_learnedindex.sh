#!/bin/bash

mkdir -p ./data/learned_index/model_lognormal

echo "================ train learned indexes on the whole dataset ================="
python2.7 ./training/learned_index.py

echo "================ train leanrned indexes on the smapled data ================="
python2.7 ./training/train_insert_learned_indexes.py
