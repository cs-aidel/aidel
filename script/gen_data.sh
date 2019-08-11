#!/bin/bash

mkdir ./data

echo "==================Generate lognormal data: 190M=============="
./build/gen_lognormal

echo "=================== sample and disorder data ======================"
./build/gen_data
