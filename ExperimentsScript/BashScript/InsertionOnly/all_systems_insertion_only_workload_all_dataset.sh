#!/bin/bash


########### global variable ############
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/yourGFEDriverPath/build"

# HAL system build path
halBuildPath="/local/user/YourHalPath/build"

# Sortledton system build path
sortledtonBuildPath="/local/user/YourSortledtonPath/build"

# Livegraph system build path
livegraphBuildPath="/local/user/YourLiveGraphPath/build"

# Teseo system build path
teseoBuildPath="/local/user/YourTeseoPath/build"

# All systems -l variable in gfe_driver

hal="our"

sortledton="sortledton.4"

livegraph="livegraph3_ro"

teseo="teseo.13"

####### clear results file #######

sudo rm /local/user/insertion-only.txt

sudo rm /local/user/insertion-only-workload-five-times-median.csv

########### systems related commands
cd "$gfeDriverPath"


########### HAL System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb="${halBuildPath}" && sudo make -j

for n in {1..5};
do
	./gfe_driver -G "${filepath}/graph500-22.properties" -u  -l "${hal}" -w 40  -d results.sqlite3
	./gfe_driver -G "${filepath}/graph500-24.properties" -u  -l "${hal}" -w 40  -d results.sqlite3
	./gfe_driver -G "${filepath}/graph500-26.properties" -u  -l "${hal}" -w 40  -d results.sqlite3
	./gfe_driver -G "${filepath}/dota-league.properties" -u  -l "${hal}" -w 40  -d results.sqlite3
	./gfe_driver -G "${filepath}/uniform-22.properties" -u   -l "${hal}" -w 40  -d results.sqlite3
	./gfe_driver -G "${filepath}/uniform-24.properties" -u   -l "${hal}" -w 40  -d results.sqlite3
	./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${hal}" -G "${filepath}/yahoo-song.el" -w 40 -r 32 
#	./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${hal}" -G "${filepath}/yahoo-song.el" -w 40 -r 32 --is_timestamped true 
	./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${hal}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32
#        ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${hal}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32 --is_timestamped true
done

########### Sortledton System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-sortledton="${sortledtonBuildPath}" && sudo make -j

for n in {1..5};
do
  	./gfe_driver -G "${filepath}/graph500-22.properties" -u  -l "${sortledton}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/graph500-24.properties" -u  -l "${sortledton}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/graph500-26.properties" -u  -l "${sortledton}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/dota-league.properties" -u  -l "${sortledton}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/uniform-22.properties" -u   -l "${sortledton}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/uniform-24.properties" -u   -l "${sortledton}" -w 40  -d results.sqlite3
    ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${sortledton}" -G "${filepath}/yahoo-song.el" -w 40 -r 32
  # ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${sortledton}" -G "${filepath}/yahoo-song.el" -w 40 -r 32 --is_timestamped true
    ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${sortledton}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32
 #  ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${sortledton}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32 --is_timestamped true
done

####### Teseo System five times run #########

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-teseo="${teseoBuildPath}" && sudo make -j

for n in {1..5};
do
  	./gfe_driver -G "${filepath}/graph500-22.properties" -u  -l "${teseo}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/graph500-24.properties" -u  -l "${teseo}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/graph500-26.properties" -u  -l "${teseo}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/dota-league.properties" -u  -l "${teseo}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/uniform-22.properties" -u   -l "${teseo}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/uniform-24.properties" -u   -l "${teseo}" -w 40  -d results.sqlite3
    ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${teseo}" -G "${filepath}/yahoo-song.el" -w 40 -r 32
   #./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${teseo}" -G "${filepath}/yahoo-song.el" -w 40 -r 32 --is_timestamped true
    ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${teseo}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32
    #./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${teseo}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32 --is_timestamped true
done

####### Livegraph System five times run #########

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-livegraph="${livegraphBuildPath}" && sudo make -j

for n in {1..5};
do
  	./gfe_driver -G "${filepath}/graph500-22.properties" -u  -l "${livegraph}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/graph500-24.properties" -u  -l "${livegraph}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/graph500-26.properties" -u  -l "${livegraph}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/dota-league.properties" -u  -l "${livegraph}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/uniform-22.properties" -u   -l "${livegraph}" -w 40  -d results.sqlite3
    ./gfe_driver -G "${filepath}/uniform-24.properties" -u   -l "${livegraph}" -w 40  -d results.sqlite3
    ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${livegraph}" -G "${filepath}/yahoo-song.el" -w 40 -r 32
    #./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${livegraph}" -G "${filepath}/yahoo-song.el" -w 40 -r 32 --is_timestamped true
    ./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${livegraph}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32
    #./gfe_driver  -u  -R 0 -d ./results.sqlite3 -l "${livegraph}" -G "${filepath}/edit-enwiki.el" -w 40 -r 32 --is_timestamped true
done

######### Calculate five times median of all systems ###########


cd "$halBuildPath"

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 1

