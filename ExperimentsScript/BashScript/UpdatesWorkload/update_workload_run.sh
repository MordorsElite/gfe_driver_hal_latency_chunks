#!/bin/bash

####### clear results file #######

sudo rm /local/user/update_workload.txt

sudo rm /local/user/updates-only-workload-five-times-median.csv

./update_workload_hal.sh  # HAL library run

./update_workload_sortledton.sh # Sortledton library run

./update_workload_teseo.sh # Teseo library run

./update_workload_livegraph.sh # livegraph library run

cd /local/user/YourHalPath/build
cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 3
