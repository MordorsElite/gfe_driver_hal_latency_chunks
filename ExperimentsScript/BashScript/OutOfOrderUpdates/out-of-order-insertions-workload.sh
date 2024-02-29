#!/bin/bash

########### global variable ############
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/YourGFEDriverPath/build"

# HAL system build path
halBuildPath   ="/local/user/YourHalPath/build"
halVBuildPath  ="/local/user/YourHalVPath/build"
# All systems -l variable in gfe_driver

hal="our"

####### clear results file #######

sudo rm /local/user/out-of-order-workload.txt

sudo rm /local/user/out-of-order-workload-five-times-median.csv

########### systems related commands
cd "$gfeDriverPath"

########################## HAL system #################################

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb="${halBuildPath}" && sudo make -j

for n in {1..5};
do
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-0.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-1.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-2.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-3.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-4.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-5.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-6.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-7.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-8.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-9.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
done

###################### HAL-V #########################################

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb="${halVBuildPath}" && sudo make -j

for n in {1..5};
do
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-0.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-1.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-2.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-3.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-4.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-5.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-6.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-7.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-8.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/out-of-order/graph500-24t-9.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 1
done

######### Calculate five times median of all systems ###########
cd "$halBuildPath"

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 5
