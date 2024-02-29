#!/bin/bash

####### clear results file #######

sudo rm /local/user/update_workload_per_second_operation.txt

sudo rm /local/user/update_workload_per_ten_second_space.txt

sudo rm /local/user/updates-only-workload-space-cal.csv

./update_workload_hal_mem.sh  # HAL library run

./update_workload_sortledton_mem.sh # Sortledton library run

./update_workload_teseo_mem.sh # Teseo library run

./update_workload_livegraph.sh # livegraph library run

cd /local/user/YourHalPath/build

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 6
