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

####### clear results file #######

cd /local/user/YourHalPath/build

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

########### systems related commands

cd /local/user/YourGFEDriverPath/build

########### HAL System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb=/local/user/YourHalPath/build && sudo make -j

./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u --log /local/user/graph500-24-1.0.graphlog -l our -w 40 -d results.sqlite3 --aging_memfp  --aging_memfp_physical  --aging_release_memory false


