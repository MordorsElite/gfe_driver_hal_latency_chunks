#!/bin/bash

####### clear results file #######

sudo rm /local/user/analytics_only_result.txt

sudo rm /local/user/analytics-only-workload-five-times-median.csv

# hal
./hal_only_analytics.sh

# sortledton
./sortledton_analytics_only.sh

# teseo sparse
./teseo_analytics_only.sh

# livegraph
./livegraph_analytics_only.sh

cd /local/user/YourHalPath/build

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 2

