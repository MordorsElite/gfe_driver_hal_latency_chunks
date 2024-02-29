//
// Created by per on 03.02.21.
//

#include "halb_driver.hpp"

#include <chrono>
#include <optional>
#include <omp.h>
#include <mutex>

#include "common/timer.hpp"
#include "utility/timeout_service.hpp"
//#define COUT_DEBUG_FORCE(msg) { std::mutex   lockss; lockss.lock(); std::cout<<msg<<std::endl;lockss.unlock();}

//#define DEBUG_LCC
#if defined(DEBUG_LCC)
#define COUT_DEBUG_LCC(msg)
#else

#endif
//#include "data-structure/data_types.h"
/*#include "algorithms/SSSP.h"
#include "algorithms/PageRank.h"
#include "algorithms/WCC.h"
#include "algorithms/CDLP.h"
#include "algorithms/LCC.h"
#include "algorithms/GAPBSAlgorithms.h"
*/
//#include "data-structure/EdgeDoesNotExistsPrecondition.h"
#include "DataLoad/InputEdge.h"
//#include "SharedLib.h"
//#define HAL reinterpret_cast<HBALStore*>(m_pImpl)

using namespace common;

//namespace gfe { extern mutex _log_mutex [[maybe_unused]]; }


namespace gfe::library {

    HalbDriver::HalbDriver(int writer_thread, int ooo, bool isDuplicateSupport, bool is_read_write, bool NODELETEOOO){
        // todo add adjacency list here
        if(ooo > 0)
        {
            txn = new TransactionManager(56, writer_thread, 1, isDuplicateSupport);

        }
        else
        {
            txn = new TransactionManager(56, writer_thread, 0, isDuplicateSupport);

        }
        ds = txn->getGraphDataStore(false,0);
        ds->IsReadWrite = is_read_write;
        ds->NODELETEOOO = NODELETEOOO;
        ga = txn->getMemory();
        read_version = 0;
    }

    HalbDriver::~HalbDriver() {
        delete txn;
        txn = nullptr;
    }

    void HalbDriver::on_main_init(int num_threads) {
        // tm.reset_max_threads(num_threads);
    }

    void HalbDriver::on_thread_init(int thread_id) {
        //  tm.register_thread(thread_id);
    }

    uint64_t HalbDriver::on_thread_destroy(int thread_id)
    {
        // tm.deregister_thread(thread_id);
       // std::cout<<"thread id: "<<thread_id<<std::endl;
       if(thread_id == -1)
       {
          // std::cout<<"hello"<<std::endl;
          return ds->getMemoryConsumptionBreakDownUpdates_free_allocator_space(ga);
       }
       else
       {
           return 0;
       }
    }

    void HalbDriver::dump_ostream(std::ostream &out) const {
        // throw NotImplemented();
    }

    uint64_t HalbDriver::num_edges() const
    {
        //  throw NotImplemented();
        // todo number of edges
       // HalbDriver *non_const_this = const_cast<HalbDriver *>(this);
        HBALStore *ds_read              = const_cast<HBALStore *>(txn->getGraphDataStore(false, read_version));
        uint64_t max_vertex_id  = ds_read->get_high_water_mark();
     //   LOG("Number of edges exist: " << max_vertex_id);
       // std::cout<<"max vertex: "<<max_vertex_id<<std::endl;
        int num_edges = 0;
        for(uint64_t v=0;v<max_vertex_id;v++)
        {
            num_edges += ds_read->perSourceGetDegree(v, 0, NULL);
            if(num_edges < 0)
            {
                std::cout<<num_edges<<" negative"<<std::endl;
            }
        }
        return num_edges;
    }

    uint64_t HalbDriver::num_vertices() const {
        /* SortledtonDriver *non_const_this = const_cast<SortledtonDriver *>(this);
         SnapshotTransaction tx = non_const_this->tm.getSnapshotTransaction(ds, false);
         auto num_vertices = tx.vertex_count();
         non_const_this->tm.transactionCompleted(tx);
         return num_vertices;*/
        //HalbDriver *non_const_this = const_cast<HalbDriver *>(this);
        HBALStore *ds = txn->getGraphDataStore(false,0);
        return ds->getMaxSourceVertex();
    }

/**
 * Returns true if the given vertex is present, false otherwise
 */
    bool HalbDriver::has_vertex(uint64_t vertex_id) const {
        /* SortledtonDriver *non_const_this = const_cast<SortledtonDriver *>(this);
         SnapshotTransaction tx = non_const_this->tm.getSnapshotTransaction(ds,
                                                                            false);  // TODO weights currently not supported
         auto has_vertex = tx.has_vertex(vertex_id);
         non_const_this->tm.transactionCompleted(tx);
         return has_vertex; */
        return true;
    }

/**
 * Returns the weight of the given edge is the edge is present, or NaN otherwise
 */
    double HalbDriver::get_weight(uint64_t source, uint64_t destination) const {
        /* SortledtonDriver *non_const_this = const_cast<SortledtonDriver *>(this);
         SnapshotTransaction tx = non_const_this->tm.getSnapshotTransaction(ds, false);

         if (!tx.has_vertex(source) || !tx.has_vertex(destination)) {
             return nan("");
         }
         weight_t w;
         auto has_edge = tx.get_weight({static_cast<dst_t>(source), static_cast<dst_t>(destination)}, (char *) &w);
         non_const_this->tm.transactionCompleted(tx);
         return has_edge ? w : nan(""); */
        return 0;
    }

/**
 * Check whether the graph is directed
 */
    bool HalbDriver::is_directed() const {
        return m_is_directed;
    }

/**
 * Impose a timeout on each graph computation. A computation that does not terminate by the given seconds will raise a TimeoutError.
 */
    void HalbDriver::set_timeout(uint64_t seconds) {
        m_timeout = std::chrono::seconds{seconds};
    }

/**
 * Add the given vertex to the graph
 * @return true if the vertex has been inserted, false otherwise (that is, the vertex already exists)
 */
    bool HalbDriver::add_vertex(uint64_t vertex_id) {

        ds->InsertSrcVertex(vertex_id, ga, 0);
        return false;
    }

/**
 * Remove the mapping for a given vertex. The actual internal vertex is not removed from the adjacency list.
 * @param vertex_id the vertex to remove
 * @return true if a mapping for that vertex existed, false otherwise
 */
    bool HalbDriver::remove_vertex(uint64_t vertex_id) {
        // throw NotImplemented();
    }


/**
 * Adds a given edge to the graph if both vertices exists already
 */
    bool HalbDriver::add_edge(gfe::graph::WeightedEdge e) {
        /*assert(!m_is_directed);
        edge_t internal_edge{static_cast<dst_t>(e.source()), static_cast<dst_t>(e.destination())};

        thread_local optional <SnapshotTransaction> tx_o = nullopt;
        if (tx_o.has_value()) {
            tm.getSnapshotTransaction(ds, true, *tx_o);
        } else {
            tx_o = tm.getSnapshotTransaction(ds, true);
        }
        auto tx = *tx_o;

        VertexExistsPrecondition pre_v1(internal_edge.src);
        tx.register_precondition(&pre_v1);
        VertexExistsPrecondition pre_v2(internal_edge.dst);
        tx.register_precondition(&pre_v2);
        // Even in the undirected case, we need to check only for the existence of one edge direction to ensure consistency.
        EdgeDoesNotExistsPrecondition pre_e(internal_edge);
        tx.register_precondition(&pre_e);

        // test
        bool inserted = true;
        try {
            tx.insert_edge(internal_edge, (char *) &e.m_weight, sizeof(e.m_weight));
            tx.insert_edge({internal_edge.dst, internal_edge.src}, (char *) &e.m_weight, sizeof(e.m_weight));
            inserted &= tx.execute();
        } catch (VertexDoesNotExistsException &e) {
            inserted = false;
        } catch (EdgeExistsException &e) {
            inserted = false;
        }
        tm.transactionCompleted(tx);
        */
        return false;
    }

    bool HalbDriver::add_edge_v2(gfe::graph::WeightedEdge e, int thread_id)
    {
        assert(!m_is_directed);
        //count++;
       // std::cout<<"insertion : "<<e.source()<<" "<<e.destination()<<std::endl;

        // std::cout<<"insertion : "<<e.source()<<" "<<e.destination()<<" "<<e.srcTimestamp<<" "<< e.m_weight<<std::endl;
        InputEdge ie{e.source(),e.destination(),e.srcTimestamp, e.m_weight};

        //
        if(e.source() != e.destination())
        {
            ds->executeEdgeInsertion(ie, ga, thread_id);
        }

        //

/*   thread_local optional <SnapshotTransaction> tx_o = nullopt;
           edge_t internal_edge{static_cast<dst_t>(e.source()), static_cast<dst_t>(e.destination())};

   //      bool insertion = true;
   //      if (tx_o.has_value()) {
   //        tm.getSnapshotTransaction(ds, false, *tx_o);
   //        auto tx = *tx_o;
   //
   //        bool exists = tx.has_edge(internal_edge);
   //        bool exists_reverse = tx.has_edge({internal_edge.dst, internal_edge.src});
   //        if (exists != exists_reverse) {
   //          cout << "Edge existed in only one direction" << endl;
   //        }
   //        if (exists) {
   //          insertion = false;
   //        }
   //        tm.transactionCompleted(tx);
   //      }

           if (tx_o.has_value()) {
               tm.getSnapshotTransaction(ds, true, *tx_o);
           } else {
               tx_o = tm.getSnapshotTransaction(ds, true);
           }
           auto tx = *tx_o;

           tx.use_vertex_does_not_exists_semantics();

           tx.insert_vertex(internal_edge.src);
           tx.insert_vertex(internal_edge.dst);

           tx.insert_edge({internal_edge.dst, internal_edge.src}, (char *) &e.m_weight, sizeof(e.m_weight));
           tx.insert_edge(internal_edge, (char *) &e.m_weight, sizeof(e.m_weight));

           bool inserted = true;
           inserted &= tx.execute();

           tm.transactionCompleted(tx);
   */
//      tm.getSnapshotTransaction(ds, false, *tx_o);
//      tx = *tx_o;
//      double out;
//      double out_reverse;
//      bool exists = tx.get_weight(internal_edge, (char*) &out);
//      bool exists_reverse = tx.get_weight({internal_edge.dst, internal_edge.src}, (char*) &out_reverse);
//      if (!exists) {
//        cout << "Forward edge does not exist." << endl;
//      }
//      if (!exists_reverse) {
//        cout << "Backward edge does not exist." << endl;
//      }
//      if (out != out_reverse) {
//        cout << "Edge sites have unequal weight: " << out << " " << out_reverse << endl;
//        cout << "This was an insertion: " << insertion << endl;
//        cout << "In neighbourhood: " << tx.neighbourhood_size(internal_edge.src) <<
//        " " << tx.neighbourhood_size(internal_edge.dst) << endl;
//      }
//      if (out != e.m_weight) {
//        cout << "Weight incorrect: " << e.m_weight << " " << out << endl;
//        cout << "This was an insertion: " << insertion << endl;
//      }
//      tm.transactionCompleted(tx);
        return true;
    }

    bool HalbDriver::remove_edge(gfe::graph::Edge e, int thread_id) {
        assert(!m_is_directed);

        InputEdge ie{e.source(),e.destination(),0,0};
        ds->executeEdgeDeletion(ie,ga, thread_id);

        return true;
    }
    bool HalbDriver::remove_edge_v2(gfe::graph::WeightedEdge e, int thread_id)
    {
        assert(!m_is_directed);

        InputEdge ie{e.source(),e.destination(),e.srcTimestamp, e.m_weight};
        ds->executeEdgeDeletion(ie,ga, thread_id);
       // ds->removeEdge(ie,ga, thread_id);
       // ds->removeEdge({e.destination(),e.source(),e.srcTimestamp, e.m_weight},ga, thread_id);

        return true;
    }

     std::vector <std::pair<uint64_t, uint>> HalbDriver::translate_bfs(std::unique_ptr<int64_t[]> &values) {
        auto N = ds->get_high_water_mark();

        std::vector <std::pair<vertex_t, uint>> logical_result(N);

#pragma omp parallel for
        for (uint v = 0; v < N; v++)
        {
            if (ds->has_vertex(v))
            {
                if (values[v] >= 0)
                {
                    logical_result[v] = std::make_pair(ds->logical_id(v), values[v]);
                }
                else
                {
                    logical_result[v] = std::make_pair(ds->logical_id(v), std::numeric_limits<uint>::max());
                }
            }
            else
            {
                logical_result[v] = std::make_pair(v, std::numeric_limits<uint>::max());
            }
        }
        return logical_result;
    }
    void HalbDriver::run_gc() {
        /*  if (!gced) {
              Timer t;
              t.start();
              ds->gc_all();
              gced = true;
              cout << "Running GC took: " << t;
          }*/
        // throw NotImplemented();
    }

    /* static void save_bfs(vector <std::pair<uint64_t, uint>> &result, const char *dump2file) {
         assert(dump2file != nullptr);
         COUT_DEBUG("save the results to: " << dump2file)

         fstream handle(dump2file, ios_base::out);
         if (!handle.good()) ERROR("Cannot save the result to `" << dump2file << "'");

         for (const auto &p : result) {
             handle << p.first << " ";

             // if  the vertex was not reached, the algorithm sets its distance to < 0
             if (p.second == numeric_limits<uint>::max()) {
                 handle << numeric_limits<int64_t>::max();
             } else {
                 handle << (int64_t) p.second;
             }
             handle << "\n";
         }
         handle.close();
     }
 */
    // todo think about the implementation
/*
    static vector <pair<uint64_t, uint>> translate_bfs(SnapshotTransaction &tx, pvector <int64_t> &values) {
        auto N = values.size();

        vector <pair<vertex_id_t, uint>> logical_result(N);

#pragma omp parallel for
        for (uint v = 0; v < N; v++) {
            if (tx.has_vertex_p(v)) {
                if (values[v] >= 0) {
                    logical_result[v] = make_pair(tx.logical_id(v), values[v]);
                } else {
                    logical_result[v] = make_pair(tx.logical_id(v), numeric_limits<uint>::max());
                }
            } else {
                logical_result[v] = make_pair(v, numeric_limits<uint>::max());
            }
        }
        return logical_result;
    }
*/
// ## start from here


    void HalbDriver::bfs(uint64_t source_vertex_id, const char *dump2file) {
        m_is_directed = false;
        if(m_is_directed) { ERROR("This implementation of the BFS does not support directed graphs"); }
       // std::cout<<"hello :"<<read_version<<std::endl;

        txn->addReadThread(read_version);

        //std::cout<<"hello 1:"<<read_version<<std::endl;

        ds = txn->getGraphDataStore(true, read_version);
       // std::cout<<"hello 2:"<<read_version<<std::endl;


        // Init
        utility::TimeoutService timeout { m_timeout };
        Timer timer; timer.start();
        //lg::Transaction transaction = m_read_only ? LiveGraph->begin_read_only_transaction() : LiveGraph->begin_transaction();

      //  uint64_t max_vertex_id = ds->getMaxSourceVertex();//bfs->get_max_vertex_id();
        //uint64_t num_vertices = m_num_vertices;
        // uint64_t num_edges = ds->CalculateNumberOfEdges();
        //uint64_t root = source_vertex_id;//ext2int(external_source_id);

       // COUT_DEBUG_BFS("root: " << root << " [external vertex: " << external_source_id << "]");
        //uint64_t num_vertices = -1;
       // u_int64_t num_vertices = ds->CalculateNumberOfVertices();
        auto physical_src = ds->physical_id(source_vertex_id);
       // uint64_t max_vertex_id = ds->get_high_water_mark();

        // Run the BFS algorithm
        auto distances = ds->do_bfs(0, 0, physical_src,15,18, read_version);

        txn->finishTransaction();
        // if(dump2file != nullptr) // store the results in the given file
        //  save_results<int64_t, false>(external_ids, dump2file);
    }

    // page rank
    void HalbDriver::pagerank(uint64_t num_iterations, double damping_factor, const char *dump2file)
    {
        m_is_directed = false;
        if(m_is_directed) { ERROR("This implementation of PageRank does not support directed graphs"); }
        txn->addReadThread(read_version);
        ds = txn->getGraphDataStore(true, read_version);
        // Run the PageRank algorithm
        std::unique_ptr<double[]> ptr_result = ds->do_pagerank(0, num_iterations, damping_factor, read_version, ga);
        txn->finishTransaction();

        // if(timeout.is_timeout()){ transaction.abort(); RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer);  }

        // Retrieve the external node ids
        //  auto external_ids = translate(&transaction, ptr_result.get(), max_vertex_id);
        //  transaction.abort(); // read-only transaction, abort == commit
        //  if(timeout.is_timeout()){ RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer); }

        // Store the results in the given file
        //  if(dump2file != nullptr)
        //    save_results(external_ids, dump2file);
    }

    // wcc
    void HalbDriver::wcc(const char *dump2file) {

        utility::TimeoutService timeout { m_timeout };
        Timer timer; timer.start();
        uint64_t max_vertex_id =  ds->get_high_water_mark();
       // size_t get_vertex_count(vertex_t version);

        //size_t get_high_water_mark();
        // run wcc

        std::unique_ptr<uint64_t[]> ptr_components = ds->do_wcc(max_vertex_id);

        // if(timeout.is_timeout()){ transaction.abort(); RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer); }

        // translate the vertex IDs
        // auto external_ids = translate(&transaction, ptr_components.get(), max_vertex_id);
        // transaction.abort(); // read-only transaction, abort == commit
        // if(timeout.is_timeout()){ RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer); }

        // store the results in the given file
        // if(dump2file != nullptr)
        //       save_results(external_ids, dump2file);

    }

    // cdlp
    void HalbDriver::cdlp(uint64_t max_iterations, const char *dump2file) {



        utility::TimeoutService timeout { m_timeout };
        Timer timer; timer.start();
        //lg::Transaction transaction = m_read_only ? LiveGraph->begin_read_only_transaction() : LiveGraph->begin_transaction();

        uint64_t max_vertex_id = ds->get_high_water_mark();

        // Run the CDLP algorithm
        std::unique_ptr<uint64_t[]> labels = ds->do_cdlp(max_vertex_id, false, max_iterations);


        // if(timeout.is_timeout()){ transaction.abort(); RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer);  }

        // Translate the vertex IDs
        //   auto external_ids = translate(&transaction, labels.get(), max_vertex_id);
        //   transaction.abort(); // read-only transaction, abort == commit
        //  if(timeout.is_timeout()){ RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer); }

        // Store the results in the given file
        //  if(dump2file != nullptr)
        //      save_results(external_ids, dump2file);

    }

// sssp

    void HalbDriver::sssp(uint64_t source_vertex_id, const char *dump2file)
    {

        double delta = 2.0;
        txn->addReadThread(read_version);
        uint64_t max_vertex_id = ds->get_high_water_mark();

       // uint64_t num_edges     = ds->CalculateNumberOfEdges();

        auto physical_src = ds->physical_id(source_vertex_id);

        auto     distances     = ds->do_sssp(0, max_vertex_id, physical_src, delta,read_version,ga);

       // auto external_ids = translate<double>(ds, distances);
        /* tm.register_thread(0);
         SnapshotTransaction tx = tm.getSnapshotTransaction(ds, false);

         run_gc();

         auto physical_src = tx.physical_id(source_vertex_id);

         auto distances = SSSP::gabbs_sssp(tx, physical_src, 2.0);

         auto external_ids = translate<double>(tx, distances);

         tm.transactionCompleted(tx);

         if (dump2file != nullptr) {
             save_result<double>(external_ids, dump2file);
         }
         tm.deregister_thread(0);*/
        //throw NotImplemented();

    }

    // do lcc



    // lcc
    void HalbDriver::lcc(const char *dump2file)
    {
        // if(m_is_directed) { ERROR("Implementation of LCC supports only undirected graphs"); }
       // std::cout<<"hello"<<std::endl;
        utility::TimeoutService timeout { m_timeout };

        Timer timer; timer.start();
        //lg::Transaction transaction = m_read_only ? LiveGraph->begin_read_only_transaction() : LiveGraph->begin_transaction();

        uint64_t max_vertex_id = ds->get_high_water_mark();

        // Run the LCC algorithm
        std::unique_ptr<double[]> scores = ds->do_lcc_undirected(max_vertex_id, ga);

      /*  double sum = 0.0;
        for(int i=0;i<(max_vertex_id);i++)
        {
            sum += scores[i];
        }
        std::cout<<"sum :"<<sum<<std::endl;*/
        // if(timeout.is_timeout()){ transaction.abort(); RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer);  }

        // Translate the vertex IDs
        //  auto external_ids = translate(&transaction, scores.get(), max_vertex_id);
        //  transaction.abort(); // read-only transaction, abort == commit
        //   if(timeout.is_timeout()){ RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer); }

        // Store the results in the given file
        //  if(dump2file != nullptr)
        //     save_results(external_ids, dump2file);*/
    }


        bool HalbDriver::can_be_validated() const {
        return true;
    }



}