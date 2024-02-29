#!/bin/bash

####### clear results file #######

sudo rm /local/user/insertion-only-scalability.txt

sudo rm /local/user/insertion-only-scalability-five-times-median.csv

./hal_insertion_only_workload_scalability.sh  # HAL library run

./sortledton_insertion_only_workload_scalability.sh # Sortledton library run

./teseo_insertion_only_workload_scalability.sh # Teseo library run

./livegraph_insertion_only_workload_scalability.sh # livegraph library run

cd /tmp/tmp.OGb4BQmGdU/build

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 7
