#!/bin/bash

mkdir -p ./data/lognormal_190M/model/

echo "================ train aidel on the whole dataset================="
python2.7 ./training/train_aidel.py

echo "================ train aidel on the sampled data==================="
python2.7 ./training/train_insert_aidel.py
