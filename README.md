---
GFE Driver
---

We extend the GFE (Graph Framework Evaluation) driver developed by Teseo (link) and extended by Sortledton (link). Basically, GFE Driver is the program used to run the experiments in "HAL: Dynamic Graph Databases with Out-of-Order Updates" by
measuring the throughput of updates in libraries supporting structural dynamic graphs and the completion times of the [Graphalytics kernels] (https://github.com/ldbc/ldbc_graphalytics).
For our comparison, we used GFE drivers to support the following systems: [HAL](git), [Sortledton](https://gitlab.db.in.tum.de/per.fuchs/sortledton), [Teseo](https://github.com/cwida/teseo), 
[LiveGraph](https://github.com/thu-pacman/LiveGraph-Binary).
It can run three kinds of experiments: insert all edges in a random permuted order from an input graph, execute the updates specified by a [graphlog file] (https://github.com/whatsthecraic/graphlog), and run BFS, PageRank (PR), weighted shortest paths (SSSP), weakly connected components (WCC), and
community detection through label propagation (CDLP) graph analytics algorithm.

### Build 

#### Requisites 
- O.S. Linux
- Autotools, [Autoconf 2.69+](https://www.gnu.org/software/autoconf/)
- A C++17 compliant compiler with support for OpenMP. We tested it with GCC 10.
- libnuma 2.0 +
- [libpapi 5.5 +](http://icl.utk.edu/papi/)
- [SQLite 3.27 +](https://sqlite.org)
- Intel Threading Building Blocks 2 (version 2020.1-2)
- Disable NUMA balancing feature to avoid the Linux Kernel to swap pages during insertions: `echo 0 | sudo tee  /proc/sys/kernel/numa_balancing`

#### Configure

Initialise the sources and the configure script by:

```
git clone https://github.com/cedar/gfe_driver
cd gfe_driver
git submodule update --init
mkdir build && cd build
autoreconf -iv ..
```

The driver needs to be linked with the system to evaluate, which has to be built ahead. 
We do not recommend linking the driver with multiple systems at once, 
due to the usage of global variables in some systems and other naming clashes. 
Instead, it is safer to reconfigure and rebuild the driver each time for a single specific system.
##### Sortledton
Use the branch `master` from `https://gitlab.db.in.tum.de/per.fuchs/sortledton`.
For the paper, we evaluated commit "a32b8ac208bb889b518e14b1317957c9a8c466b6".

Follow the instructions in the README of the repository to setup and build the library.
Then configure the driver with:

```
mkdir build && cd build
../configure --enable-optimize --disable-debug --with-sortledton=/path/to/microbenchmark/build   
```

##### LiveGraph

The binary file of LiveGraph is not working; hence, download the original source code from the [official repository] (https://github.com/thu-pacman/LiveGraph). Then configure the driver by pointing the path to where the library has been downloading:

```
mkdir build && cd build
cmake .. && make
```

##### Teseo

Use the branch `master` from https://github.com/cwida/teseo.
In the paper, we evaluated version `14227577731d6369b5366613f3e4a679b1fd7694`.

```
git clone https://github.com/cwida/teseo
cd teseo
./autoreconf -iv
mkdir build && cd build
../configure --enable-optimize --disable-debug
make -j
```

If the build has been successful, it should at least create the archive `libteseo.a`.
Then configure the driver with:

```
mkdir build && cd build
../configure --enable-optimize --disable-debug --with-teseo=/path/to/teseo/build   
```

##### HAL
Use the branch `master` from `https://gitlab.db.in.tum.de/user/HAL`.

Follow the instructions in the README of the repository to setup and build the library.
Then configure the driver with:

```
mkdir build && cd build
cmake .. && make  
```
#### Compile

Once configured, run `make -j`. There is no `install` target, the final artifact is the executable `gfe_driver`. 

### Datasets

In our experiments, we used the following input graphs and data sets:

- `dota-league` and `graph500-SF`, with `SF` in {22, 24 26}, were taken from the [official Graphalytics collection](https://www.graphalytics.org/datasets).
- `uniform-SF`, with `SF` in {22, 24} were generated with an [ad-hoc tool](https://github.com/whatsthecraic/uniform_graph_generator). These are synthetic graphs having the same number of vertices and edges of `graph500-SF`, but a uniform node degree distribution.
- The logs for the experiments with updates, i.e. with both insertions and deletions,
  were generated with another [ad-hoc tool](https://github.com/whatsthecraic/graphlog). 
- `yahoo-songs` and `edit-enwiki` were taken from the [Konect webpage](http://konect.cc/networks/) they were prepared 
  for our experiments by sorting them by timestamp and removing duplicates by using `tools/timestampd_graph_2_edge_list.py`.  

A complete image of all datasets used in the experiments can be downloaded from Zenodo: [input graphs](https://zenodo.org/record/3966439),
[graph logs](https://zenodo.org/record/3967002), [dense friendster](https://zenodo.org/record/5146230) and [timestamped graphs](https://zenodo.org/record/5752476).

### Repeating the experiments of the paper
We have scripts in the folder ExperimentsScript/BashScript for paper experiments.
##### Figure 7: Graph500-24 scalability analysis
Run InsertionOnly/graph500-24-scalability-all-system.sh
##### Figure 8: Insertion throughput

Run InsertionOnly/all_systems_insertion_only_workload_all_dataset.sh

##### Figure 9: Memory usage on Graph500-24 update workload
Run UpdatesWorkload/update_workload_run_mem.sh

##### Figure 10: Performance evaluation on graph analytics.

Run AnalyticsWorkload/analytics_only.sh

##### Figure 11: HAL vs. Sortledton on read/write workload

Run ConcurrentReadWrite/hal_sortledton_concurrent_read_write.sh

##### Figure 12: Performance on in- and out-of-order insertions.

Run OutOfOrderUpdates/out-of-order-insertions-workload.sh

#### Figure 13: Query result size when varying the swap percentage, on Graph500-24

Run OutOfOrderUpdates/out-of-order-updates-workload-num-edges-all-systems.sh
#### Table 3: Throughput (MEPS) on graph500-24 workload
Run UpdatesWorkload/update_workload_run.sh
#### Table 4: Throughput variations when varying the swap percentage SW, on Graph500-24.

Run OutOfOrderUpdates/out-of-order-updates-workload.sh
