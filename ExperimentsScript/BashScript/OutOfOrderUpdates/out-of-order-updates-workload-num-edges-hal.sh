#!/bin/bash
cd /local/user/YourGFEDriverPath/build

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-halb=/local/user/YourHalPath/build && sudo make -j

./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-0.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-1.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-2.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-3.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-4.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-5.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-6.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-7.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-8.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-9.dat  -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
./gfe_driver -G /local/user/graph500-24-workload1/graph500-24-workload/duplicate-workload/graph500-24-updates-10.dat -u  -l our -w 1 -d results.sqlite3 --ooo 3 --ids true
