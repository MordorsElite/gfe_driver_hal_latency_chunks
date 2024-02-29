#!/bin/bash

########### global variable ############
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/YourGFEDriverPath/build"

# HAL system build path
halBuildPath="/local/user/YourHalPath/build"

# Sortledton system build path
sortledtonBuildPath="/local/user/YourSortledtonPath/build"

# Livegraph system build path
#livegraphBuildPath="/local/ghufran/LiveGraph/build"

# Teseo system build path
#teseoBuildPath="/local/ghufran/teseo/build"

# All systems -l variable in gfe_driver

hal="our"

sortledton="sortledton.4"

livegraph="livegraph3_ro"

teseo="teseo.13"

####### clear results file #######
sudo rm /local/user/mixed_update_analytics_workload.txt

sudo rm /local/user/rw-only-workload-five-times-median.csv

cd /local/user/YourHalPath/build

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

########### systems related commands
cd "$gfeDriverPath"
########### HAL System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb="${halBuildPath}" && sudo make -j

./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true

./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l "${hal}" -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true

########### Sortledton System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-sortledton="${sortledtonBuildPath}" && sudo make -j

./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true

./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
./gfe_driver  -u  -R 5 -d results.sqlite3 -l sortledton.4 -G /local/user/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/user/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true

####### Teseo System five times run #########

#cd ../ && sudo rm -r build && sudo rm configure

#mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-teseo="${teseoBuildPath}" && sudo make -j

 #   ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
  #  ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
 #   ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
 #   ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true

  #  ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
   # ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
   # ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
  #  ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${teseo}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
#done

####### Livegraph System five times run #########

#cd ../ && sudo rm -r build && sudo rm configure

#mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-livegraph="${livegraphBuildPath}" && sudo make -j

#for n in {1..5};
#do
 #   ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
  #  ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
 #   ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
  
#  ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,bfs --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true

 #   ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
  #  ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 16  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
  #  ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 16 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
   # ./gfe_driver  -u  -R 3 -d results.sqlite3 -l "${livegraph}" -G /local/ghufran/gfe_datasets/graph500-24.properties -w 20  -r 20 --blacklist sssp,cdlp,wcc,lcc,pagerank --log /local/ghufran/graph500-24-1.0.graphlog --aging_timeout 2h --mixed_workload true
#done

cd "$halBuildPath"

cd ../ & rm -r build & mkdir build & cd build & cmake .. &  make

./main 4
