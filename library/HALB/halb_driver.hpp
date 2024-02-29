//
// Created by per on 01-23-2023.
//

#pragma once

#include <fstream>
#include <assert.h>
#include <vector>

#include "third-party/libcuckoo/cuckoohash_map.hh"

#include "library/interface.hpp"

#include "TransactionManager.h"

namespace gfe::library {

#define COUT_DEBUG_FORCE(msg) { std::scoped_lock<std::mutex> lock{::gfe::_log_mutex}; std::cout << "[HalbDriver::" << __FUNCTION__ << "] " << msg << std::endl; }
#if defined(DEBUG)
#define COUT_DEBUG(msg) COUT_DEBUG_FORCE(msg)
#else
#define COUT_DEBUG(msg)
#endif

    // TODO can i implement reuse of transactions?

    class HalbDriver : public virtual UpdateInterface, public virtual GraphalyticsInterface{
        HalbDriver(const HalbDriver &) = delete;

        HalbDriver &operator=(const HalbDriver &) = delete;

    protected:
        void *m_pImpl; // pointer to the sortleton library
        TransactionManager *txn;
        HBALStore *ds;
        MemoryAllocator *ga;
       // std::mutex l;
       // std::atomic<int64_t> count = 0;
        bool m_is_directed;
        std::chrono::seconds m_timeout{0}; // the budget to complete each of the algorithms in the Graphalytics suite
        bool gced = false;

// todo check in future either I need to use this function or not
       // template <typename T>
        std::vector <std::pair<uint64_t, uint>> translate_bfs(std::unique_ptr<int64_t[]> &values);
        template <typename T>
        std::vector<std::pair<uint64_t, T>> translate(HBALStore *tx, std::vector<T>& values) {
            int N = values.size();

            std::vector<std::pair<vertex_t , T>> logical_result(N);

#pragma omp parallel for
            for (uint v = 0; v <  N; v++) {
                if (tx->has_vertex(v)) {
                    logical_result[v] = std::make_pair(tx->logical_id(v), values[v]);
                } else {
                    logical_result[v] = std::make_pair(v, std::numeric_limits<uint>::max());
                }
            }
            return logical_result;
        }

        template <typename T>
        void save_result(std::vector<std::pair<uint64_t, T>> &result, const char *dump2file) {
            assert(dump2file != nullptr);
            COUT_DEBUG("save the results to: " << dump2file)

            std::fstream handle(dump2file, std::ios_base::out);
            if (!handle.good()) ERROR("Cannot save the result to `" << dump2file << "'");

            for (const auto &p : result) {
                handle << p.first << " ";
                handle << p.second;
                handle << "\n";
            }
            handle.close();
        }

        void run_gc();


    public:
        std::atomic<int> read_version;
        std::mutex testLock;
        //std::mutex lock;
        HalbDriver(int thread_id, int ooo, bool isDuplicateSupport, bool is_read_write = false, bool NODELETEOOO = false);

        /**
         * Destructor
         */
        virtual ~HalbDriver();

        /**
         * Dump the content of the graph to given stream
         */
        void dump_ostream(std::ostream &out) const;

        /**
         * Get the number of edges contained in the graph
         */
        virtual uint64_t num_edges() const;

        /**
         * Get the number of nodes stored in the graph
         */
        virtual uint64_t num_vertices() const;

        /**
         * Returns true if the given vertex is present, false otherwise
         */
        virtual bool has_vertex(uint64_t vertex_id) const;

        /**
         * Returns the weight of the given edge is the edge is present, or NaN otherwise
         */
        virtual double get_weight(uint64_t source, uint64_t destination) const;

        /**
         * Check whether the graph is directed
         */
        virtual bool is_directed() const;

        /**
         * Impose a timeout on each graph computation. A computation that does not terminate by the given seconds will raise a TimeoutError.
         */
        virtual void set_timeout(uint64_t seconds);

        /**
         * Add the given vertex to the graph
         * @return true if the vertex has been inserted, false otherwise (that is, the vertex already exists)
         */
        virtual bool add_vertex(uint64_t vertex_id);

        /**
         * Remove the mapping for a given vertex. The actual internal vertex is not removed from the adjacency list.
         * @param vertex_id the vertex to remove
         * @return true if a mapping for that vertex existed, false otherwise
         */
        virtual bool remove_vertex(uint64_t vertex_id);

        /**
        * Add the given edge in the graph
        * @return true if the edge has been inserted, false if this edge already exists or one of the referred
        *    vertices does not exist.
        */
        virtual bool add_edge(gfe::graph::WeightedEdge e);

        /**
        * Add the given edge in the graph. Implicitly create the referred vertices if they do not already exist
        * @return true if the edge has been inserted, false otherwise (e.g. this edge already exists)
        */
        virtual bool add_edge_v2(gfe::graph::WeightedEdge e, int thread_id=0);

        /**
         * Remove the given edge from the graph. There is no way to check whether the operation actually succeeded
         * in this implementation of GraphOne. Attempting to remove an edge that does not exist may result in a crash.
         * @return always true when both the source & the destination vertices already exist, false otherwise
         */
        virtual bool remove_edge(gfe::graph::Edge e, int thread_id);
        virtual bool remove_edge_v2(gfe::graph::WeightedEdge e, int thread_id);
        virtual void on_main_init(int num_threads);

        /**
         * Callback, invoked when a thread is created
         */
        virtual void on_thread_init(int thread_id);

        /**
         * Callback, invoked when a thread is going to be removed
         */
        virtual uint64_t on_thread_destroy(int thread_id);

        /**
         * Perform a BFS from source_vertex_id to all the other vertices in the graph.
         * @param source_vertex_id the vertex where to start the search
         * @param dump2file if not null, dump the result in the given path, following the format expected by the benchmark specification
         */
        virtual void bfs(uint64_t source_vertex_id, const char *dump2file = nullptr);
        int64_t do_bfs_BUStep(uint64_t max_vertex_id, int64_t *distances, int64_t distance, gapbs::Bitmap &front,gapbs::Bitmap &next);
        int64_t do_bfs_TDStep(uint64_t max_vertex_id, int64_t *distances, int64_t distance, gapbs::SlidingQueue<int64_t> &queue);
        void do_bfs_QueueToBitmap(uint64_t max_vertex_id, const gapbs::SlidingQueue<int64_t> &queue, gapbs::Bitmap &bm);
        void do_bfs_BitmapToQueue(uint64_t max_vertex_id, const gapbs::Bitmap &bm, gapbs::SlidingQueue<int64_t> &queue);
        std::unique_ptr<int64_t[]> do_bfs_init_distances(uint64_t max_vertex_id);
        std::unique_ptr<int64_t[]> do_bfs(uint64_t num_vertices, uint64_t max_vertex_id, uint64_t root, int alpha = 15, int beta = 18);
        /**
         * Execute the PageRank algorithm for the specified number of iterations.
         *
         * @param num_iterations the number of iterations to execute the algorithm
         * @param damping_factor weight for the PageRank algorithm, it affects the score associated to the sink nodes in the graphs
         * @param dump2file if not null, dump the result in the given path, following the format expected by the benchmark specification
         */
        virtual void pagerank(uint64_t num_iterations, double damping_factor = 0.85, const char *dump2file = nullptr);
       std::unique_ptr<double[]> do_pagerank(uint64_t max_vertex_id, uint64_t num_iterations, double damping_factor);

        /**
         * Weakly connected components (WCC), associate each node to a connected component of the graph
         * @param dump2file if not null, dump the result in the given path, following the format expected by the benchmark specification
         */
        virtual void wcc(const char *dump2file = nullptr);
      //  std::unique_ptr<uint64_t[]> do_wcc(uint64_t max_vertex_id);
        /**
         * Community Detection using Label-Propagation. Associate a label to each vertex of the graph, according to its neighbours.
         * @param max_iterations max number of iterations to perform
         * @param dump2file if not null, dump the result in the given path, following the format expected by the benchmark specification
         */
       virtual void cdlp(uint64_t max_iterations, const char *dump2file = nullptr);
      //  std::unique_ptr<uint64_t[]> do_cdlp(uint64_t max_vertex_id, bool is_graph_directed, uint64_t max_iterations);
        /**
         * Local clustering coefficient. Associate to each vertex the ratio between the number of its outgoing edges and the number of
         * possible remaining edges.
         * @param dump2file if not null, dump the result in the given path, following the format expected by the benchmark specification
         */
        virtual void lcc(const char *dump2file = nullptr);
     //   std::unique_ptr<double[]> do_lcc_undirected(HBALStore* ds1, vertex_t max_vertex_id);
        /**
         * Single-source shortest paths. Compute the weight related to the shortest path from the source to any other vertex in the graph.
         * @param source_vertex_id the vertex where to start the search
         * @param dump2file if not null, dump the result in the given path, following the format expected by the benchmark specification
         */
       virtual void sssp(uint64_t source_vertex_id, const char *dump2file = nullptr);
      //  gapbs::pvector<double> do_sssp(uint64_t num_edges, uint64_t max_vertex_id, uint64_t source, double delta);
        virtual bool can_be_validated() const;
    };

}
