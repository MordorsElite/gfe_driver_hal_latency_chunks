#!/bin/bash
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/YourGFEPath/build"

# HAL system build path
halBuildPath="/local/user/YourHalPath/build"

cd /tmp/tmp.rHEi3NouxU/build

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb="${halBuildPath}" && sudo make -j

for n in {1..5}
do
  	for NT in 1 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40; do
                ./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u -l our -w $NT -d output_results.sqlite3 -s 1
        done
done
