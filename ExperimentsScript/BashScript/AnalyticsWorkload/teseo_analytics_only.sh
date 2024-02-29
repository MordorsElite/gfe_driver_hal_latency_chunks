#!/bin/bash
cd /local/user/YourGFEPath/build
cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-teseo=/local/user/YourTeseoPath/build  && sudo make -j

# 1
./gfe_driver -G /local/user/gfe_datasets/graph500-22.properties -u  -l teseo.13 -w 40 -R 5 --blacklist lcc -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u  -l teseo.13 -w 40 -R 5 --blacklist lcc -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/graph500-26.properties -u  -l teseo.13 -w 40 -R 5 --blacklist lcc -d results.sqlite3
./gfe_driver -G /local/user/gfe_datasets/dota-league.properties -u  -l teseo.13 -w 40 -R 5 --blacklist lcc -d results.sqlite3
