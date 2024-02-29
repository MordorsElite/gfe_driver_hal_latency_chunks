#!/bin/bash

########### global variable ############
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/YourGFEPath/build"

# HAL system build path
halBuildPath="/local/user/YourHalPath/build"

# All systems -l variable in gfe_driver

hal="our"

####### clear results file #######

cd "$halBuildPath"

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

########### systems related commands
cd "$gfeDriverPath"

########### HAL System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb=/local/user/YourHalPath/build && sudo make -j

# 1
./gfe_driver -G /local/user/gfe_datasets/graph500-22.properties -u  -l our -w 40 -R 5 --blacklist lcc  -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u  -l our -w 40 -R 5 --blacklist lcc  -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-26.properties -u  -l our -w 40 -R 5 --blacklist lcc  -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/dota-league.properties -u  -l our -w 40 -R 5 --blacklist lcc -d results.sqlite3

# 2
#./gfe_driver -G /local/ghufran/gfe_datasets/graph500-22.properties -u  -l our -w 40 -R 3 --blacklist lcc  -d results.sqlite3
#./gfe_driver -G /local/ghufran/gfe_datasets/graph500-24.properties -u  -l our -w 40 -R 3 --blacklist lcc  -d results.sqlite3
#./gfe_driver -G /local/ghufran/gfe_datasets/graph500-26.properties -u  -l our -w 40 -R 3 --blacklist lcc  -d results.sqlite3
#./gfe_driver -G /local/ghufran/gfe_datasets/dota-league.properties -u  -l our -w 40 -R 3 --blacklist lcc -d results.sqlite3

# 3
#./gfe_driver -G /local/ghufran/gfe_datasets/graph500-22.properties -u  -l our -w 40 -R 3 --blacklist lcc  -d results.sqlite3
#./gfe_driver -G /local/ghufran/gfe_datasets/graph500-24.properties -u  -l our -w 40 -R 3 --blacklist lcc  -d results.sqlite3
#./gfe_driver -G /local/ghufran/gfe_datasets/graph500-26.properties -u  -l our -w 40 -R 3 --blacklist lcc  -d results.sqlite3
