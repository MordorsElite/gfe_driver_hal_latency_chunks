#!/bin/bash
cd /local/user/yourLiveGraphPath/build
cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-livegraph=/local/user/YourLiveGraphPath/build && sudo make -j


for n in {1..5}
do
  	for NT in 1 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40; do
                ./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u -l livegraph3_ro -w $NT -d output_results.sqlite3 -s 1
        done
done
