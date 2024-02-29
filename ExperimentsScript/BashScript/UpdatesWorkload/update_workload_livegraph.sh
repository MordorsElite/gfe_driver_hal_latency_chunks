#!/bin/bash
########### global variable ############
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/YourGFEDriverPath/build"

# Livegraph system build path
livegraphBuildPath="/local/user/YourLiveGraphPath/build"

livegraph="livegraph3_ro"

########### systems related commands
cd "$gfeDriverPath"

####### Livegraph System five times run #########

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-livegraph="${livegraphBuildPath}" && sudo make -j

./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u --log /local/user/graph500-24-1.0.graphlog -l "${livegraph}" -w 40 -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u --log /local/user/graph500-24-1.0.graphlog -l "${livegraph}" -w 40 -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u --log /local/user/graph500-24-1.0.graphlog -l "${livegraph}" -w 40 -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u --log /local/user/graph500-24-1.0.graphlog -l "${livegraph}" -w 40 -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u --log /local/user/graph500-24-1.0.graphlog -l "${livegraph}" -w 40 -d results.sqlite3
