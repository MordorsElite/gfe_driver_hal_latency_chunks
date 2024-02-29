
#!/bin/bash
########### global variable ############
# dataset directory path
filepath="/local/user/gfe_datasets"

# gfe driver build path
gfeDriverPath="/local/user/YourGFEDriverPath/build"

# Sortledton system build path
sortledtonBuildPath="/local/user/YourSortledtonPath/build"

sortledton="sortledton.4"

########### systems related commands
cd "$gfeDriverPath"

########### Sortledton System five times run ##############

cd ../ && sudo rm -r build && sudo rm configure

mkdir build && cd build && autoreconf -iv .. && sudo ../configure --enable-optimize --disable-debug --with-sortledton="${sortledtonBuildPath}" && sudo make -j

./gfe_driver -G /local/user/gfe_datasets/graph500-24.properties -u --log /local/user/graph500-24-1.0.graphlog -l "${sortledton}" -w 40 -d results.sqlite3 --aging_memfp  --aging_memfp_physical  --aging_release_memory false
