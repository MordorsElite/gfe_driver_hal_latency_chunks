#!/bin/bash
########### global variable ############
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/YourGFEDriverPath/build"

# HAL system build path
halBuildPath="/local/user/YourHalPath/build"

# All systems -l variable in gfe_driver

hal="our"

########### systems related commands

####### clear results file #######

sudo rm /local/user/duplicate_result.txt

sudo rm /local/user/duplicate-workload-five-times-median.csv

cd "$halBuildPath"

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

cd "$gfeDriverPath"

########### HAL System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb="${halBuildPath}" && sudo make -j

for n in {1..5};
do
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-0.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-1.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-2.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-3.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-4.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-5.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-6.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-7.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-8.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-9.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
    ./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-10.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 2
done

######### Calculate five times median of all systems ###########


cd "$halBuildPath"

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 6
