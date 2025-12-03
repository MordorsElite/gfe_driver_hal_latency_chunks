/**
 * Copyright (C) 2019 Dean De Leo, email: dleo[at]cwi.nl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <iostream>
#include "common/database.hpp"
#include "common/error.hpp"
#include "common/system.hpp"
#include "common/timer.hpp"
#include "experiment/aging2_experiment.hpp"
#include "experiment/mixed_workload.hpp"
#include "experiment/mixed_workload_result.hpp"
#include "experiment/insert_only.hpp"
#include "experiment/graphalytics.hpp"
#include "experiment/validate.hpp"
#include "graph/edge_stream.hpp"
#include "library/interface.hpp"
#include "third-party/cxxopts/cxxopts.hpp"
#include "utility/memory_usage.hpp"
#include <fstream>
#include "configuration.hpp"
#if defined(HAVE_OPENMP)
#include "omp.h"
#endif

extern "C" { void invoke_me(); }

using namespace common;
using namespace gfe;
using namespace gfe::experiment;
using namespace std;

static void run_standalone(int argc, char* argv[])
{
    std::string commonDir = "/local/user/";
    configuration().initialise(argc, argv);

    if(configuration().get_aging_memfp() && !configuration().get_aging_memfp_physical())
    {
        utility::MemoryUsage::initialise(argc, argv); // init the memory profiler
    }
   // LOG("[driver] Initialising ...");

    if(configuration().has_database())
    {
        LOG( "[driver] Save the current configuration properties in " << configuration().get_database_path() )
        configuration().save_parameters();
    }

    const std::string& path_graph = configuration().get_path_graph();
    // graph to use?
    if(path_graph.empty()) ERROR("Path to the graph to load not set (use the parameter --graph)");

    // OpenMP settings
#if defined(HAVE_OPENMP)
    if(configuration().num_threads_omp() > 0)
    {
        LOG("[driver] OpenMP, max number of threads: " << configuration().num_threads_omp());
        omp_set_num_threads(configuration().num_threads_omp());
    }
#endif

    // implementation to evaluate

    shared_ptr<library::Interface> impl { configuration().generate_graph_library() };

    impl->set_timeout(configuration().get_timeout_graphalytics());

    auto impl_ga = dynamic_pointer_cast<library::GraphalyticsInterface>(impl);
    if(impl_ga.get() == nullptr && configuration().num_repetitions() > 0)
    { // Shall we execute the Graphalytics suite?
        ERROR("The library does not support the Graphalytics suite of algorithms");
    }

   // LOG("[driver] The library is set for a directed graph: " << (configuration().is_graph_directed() ? "yes" : "no"));
    uint64_t random_vertex = numeric_limits<uint64_t>::max();
    int64_t num_validation_errors = -1; // -1 => no validation performed
    if(configuration().is_load()){
        auto impl_load = dynamic_pointer_cast<library::LoaderInterface>(impl);
        if(impl_load.get() == nullptr){ ERROR("The library `" << configuration().get_library_name() << "' does not support loading"); }
        auto impl_rndvtx = dynamic_pointer_cast<library::RandomVertexInterface>(impl);
        if(impl_rndvtx.get() == nullptr){ERROR("The library `" << configuration().get_library_name() << "' does not allow to fetch a random vertex"); }

      //  LOG("[driver] Loading the graph: " << path_graph);
        common::Timer timer; timer.start();
        impl_load->load(path_graph);
        timer.stop();
     //   LOG("[driver] Load performed in " << timer);
        if(configuration().validate_inserts() && impl_load->can_be_validated())
        {
            auto stream = make_shared<graph::WeightedEdgeStream> ( configuration().get_path_graph() );
            num_validation_errors = validate_updates(impl_load, stream);
        }
        random_vertex = impl_rndvtx->get_random_vertex_id();
    }
    else
    {
        auto impl_upd = dynamic_pointer_cast<library::UpdateInterface>(impl);
        if(impl_upd.get() == nullptr){ ERROR("The library `" << configuration().get_library_name() << "' does not support updates"); }

        if(configuration().get_update_log().empty())
        {

            LOG("[driver] Using the graph " << path_graph);
            auto stream = make_shared<graph::WeightedEdgeStream> ( configuration().get_path_graph() );

            if(configuration().measure_latency()) ERROR("[driver] InsertOnly, support for latency measurements removed");

            if(configuration().is_out_of_order_workload() < 1)
            {
                if (!configuration().is_timestamped_graph())
                {
                    //   LOG("[driver] graph is not sorted by timestamp: permuting");
                    stream->permute();
                    stream->generate_synthetic_timestamp();
                }
                else
                {
                    //LOG("[driver] graph is sorted by timestamp: no shuffling");
                }
                if(stream->num_edges() > 0) random_vertex = stream->get(0).m_source;

                LOG("[driver] Number of concurrent threads: " << configuration().num_threads(THREADS_WRITE));

                InsertOnly experiment{impl_upd, stream, configuration().num_threads(THREADS_WRITE)};
                experiment.set_build_frequency(chrono::milliseconds{configuration().get_build_frequency()});
                experiment.set_scheduler_granularity(1ull < 20);
                experiment.execute();

                if (configuration().has_database()) experiment.save(true, configuration().is_timestamped_graph());

                if (configuration().validate_inserts() && impl_upd->can_be_validated())
                {
                    num_validation_errors = validate_updates(impl_upd, stream);
                }
                // memory consumption
                if(configuration().get_library_name() == "our")
                {
                   // std::cout<<"Memory taken by hal:"<< impl_upd->on_thread_destroy(-1)<<std::endl;
                }
               // std::cout<<"Physical memory:"<<common::get_memory_footprint()<<std::endl;
            }
            else
            {
                // out-of-order updates workload
                LOG("out-of-order updates number: " << configuration().is_out_of_order_workload());

                Timer timer;
                timer.start();
                for(uint64_t pos = 0; pos < stream->num_edges(); pos++)
                {
                    auto edge = stream->get(pos);
                   // std::cout<<edge.m_source<<" "<<edge.m_destination<<" "<<edge.srcTimestamp<<" "<<edge.m_weight<<std::endl;

                    if(edge.m_weight > 0)
                    {
                        [[maybe_unused]] bool result = impl_upd->add_edge_v2(edge,0);
                        assert(result == true && "Edge not inserted");
                    }
                    else
                    {
                        impl_upd->add_vertex(edge.source());
                        impl_upd->add_vertex(edge.destination());
                        impl_upd->remove_edge_v2(edge,0);
                    }
                }
                timer.stop();

                //########### extract out-of-order dataset id (0, 1, 2,..., 10)
                std::string file_name = configuration().m_path_graph_to_load;
                //const std::string& path_graph = configuration().get_path_graph();
                // Define a regular expression pattern to match an integer before a period
                std::regex pattern("(\\d+)\\.");

                // Search for a match in the file name
                std::smatch match;
                int extracted_integer = -1;

                if (std::regex_search(file_name, match, pattern)) {
                    // Extract the matched integer
                    extracted_integer = std::stoi(match[1]);
                  //  std::cout << "Extracted Integer: " << extracted_integer << std::endl;
                }
                else
                {
                    std::cout << "Integer not found in the file name." << std::endl;
                }

                //#########

                if(configuration().is_out_of_order_workload() == 1)
                {

                    std::cout<<"Result stored in file 1"<<" "<<configuration().is_out_of_order_workload()<<std::endl;
                    Timer timer_analytics;
                    auto begin = std::chrono::high_resolution_clock::now();

                    impl_ga->pagerank(10, 0.85, "null");

                    auto end = std::chrono::high_resolution_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
                    double datastore_loading_time  = elapsed.count() * 1e-9;

                    std::cout<<"results"<<datastore_loading_time<<std::endl;
                    timer_analytics.stop();
                    const std::string output_file = commonDir + "out-of-order-workload.txt";
                    std::ofstream outfile(output_file, std::ios_base::app);
                    if (outfile.is_open()) {
                        std::cout << "File opened successfully.\n";
                        outfile<<extracted_integer<<" "<<timer.seconds()<<" "<<datastore_loading_time<<"\n";
                        outfile.close();
                    }
                    else
                    {
                        std::cerr << "Failed to open the file.\n";
                    }
                    // out-of-order different edges sensitivity analysis
                }
                else if(configuration().is_out_of_order_workload() == 2)
                {
                    // sensitivty analysis as increased duplicate insertion/deletion workload
                    std::cout<<"Result stored in file"<<" "<<configuration().is_out_of_order_workload()<<std::endl;
                    const std::string output_file = commonDir + "duplication-insertion-deletion-workload.txt";
                    std::ofstream outfile(output_file, std::ios_base::app);
                    if (outfile.is_open())
                    {
                        std::cout << "File opened successfully.\n";
                        outfile<<extracted_integer<<" "<<timer.seconds()<<"\n";
                        outfile.close();
                    }
                    else
                    {
                        std::cerr << "Failed to open the file.\n";
                    }
                }
                else
                {
                    // ############# library comparison ############
                    std::cout<<"Different systems edge count"<<std::endl;
                    int system_id = -1;
                    if(configuration().m_library_name == "our")
                    {
                        // Halb
                        system_id = 0;
                    }
                    else
                    {
                        // Sortledton
                        system_id = 1;
                    }

                    // ############ library comparison end #########

                    // count of edges for all systems
                    std::cout<<"Result stored in file 2"<<" "<<configuration().is_out_of_order_workload()<<std::endl;
                    const std::string output_file = commonDir + "duplication-insertion-deletion-workload-edge-count.txt";
                    std::ofstream outfile(output_file, std::ios_base::app);
                    if (outfile.is_open()) {
                        std::cout << "File opened successfully.\n";
                        outfile<<system_id<<" "<<extracted_integer<<" "<< impl_upd->num_edges()<<"\n";
                        outfile.close();
                    }
                    else
                    {
                        std::cerr << "Failed to open the file.\n";
                    }
                }

                LOG("Insertions performed with time: " << timer.seconds());
                uint64_t number_of_edges = (impl_upd->num_edges() / 2);
                LOG("Number of edges exist: " << number_of_edges);
            }
        }
        else
        {
            if (configuration().is_mixed_workload())
            {
              LOG("[driver] Number of write threads: " << configuration().num_threads(THREADS_WRITE));
              LOG("[driver] Number of read threads: " << configuration().num_threads(THREADS_READ));
              LOG("[driver] Aging2, path to the log of updates: " << configuration().get_update_log());
              //  std::ofstream outfile1;
              //  outfile1<<"Writer thread: "<<configuration().num_threads(THREADS_WRITE)<<" Read thread:"<<configuration().num_threads(THREADS_READ)<<"\n";//t_local.microseconds()
              // Configure aging experiment
              Aging2Experiment agingExperiment;
              agingExperiment.set_library(impl_upd);
              agingExperiment.set_log(configuration().get_update_log());
              agingExperiment.set_parallelism_degree(configuration().num_threads(THREADS_WRITE));
              agingExperiment.set_release_memory(configuration().get_aging_release_memory());
              agingExperiment.set_report_progress(true);
              agingExperiment.set_report_memory_footprint(configuration().get_aging_memfp_report());
              agingExperiment.set_build_frequency(chrono::milliseconds{configuration().get_build_frequency()});
              agingExperiment.set_max_weight(configuration().max_weight());
              agingExperiment.set_measure_latency(configuration().measure_latency());
              agingExperiment.set_num_reports_per_ops(configuration().get_num_recordings_per_ops());
              agingExperiment.set_timeout(chrono::seconds{configuration().get_timeout_aging2()});
              agingExperiment.set_measure_memfp(configuration().measure_memfp());
              agingExperiment.set_memfp_physical(configuration().get_aging_memfp_physical());
              agingExperiment.set_memfp_threshold(configuration().get_aging_memfp_threshold());
              agingExperiment.set_cooloff(chrono::seconds{configuration().get_aging_cooloff_seconds()});
              
              // Configure analytics experiment
              GraphalyticsAlgorithms properties { path_graph };
              if(properties.bfs.m_enabled == true && properties.sssp.m_enabled == false)
              {
                LOG("[driver] Enabling SSSP with random weights, source vertex: " << random_vertex);
                properties.sssp.m_enabled = true;
                properties.sssp.m_source_vertex = random_vertex;
              }

              configuration().blacklist(properties);
              GraphalyticsSequential exp_seq { impl_ga, configuration().num_repetitions(), properties };

              MixedWorkload experiment(agingExperiment, exp_seq, configuration().num_threads(ThreadsType::THREADS_READ));
              auto result = experiment.execute();
              // ########

              double num_op   = result.m_aging_result.m_num_operations_total;
              double com_time = result.m_aging_result.m_completion_time;
              double average_meps_throughput = num_op/com_time; // million edges per second
              uint64_t median_result = 0.0;
              // analytics
              int which_algo = 0;
              if(properties.bfs.m_enabled == true)
              {
                    // bfs active
                  which_algo = 0;
                 std::vector<int64_t> times_result =  result.m_graphalytics.m_exec_bfs;
                 std::sort(times_result.begin(), times_result.end());
                 size_t size = times_result.size();
                  if (size % 2 == 0)
                  {
                      size_t middle = size / 2;
                      median_result = static_cast<double>(times_result[middle - 1] + times_result[middle]) / 2.0;
                  }
                  else
                  {
                      median_result = static_cast<double>(times_result[size / 2]);
                  }

              }
              else
              {
                    // pagerank active
                  std::vector<int64_t> times_result =   result.m_graphalytics.m_exec_pagerank;
                  std::sort(times_result.begin(), times_result.end());
                  size_t size = times_result.size();
                  which_algo = 1;
                  if (size % 2 == 0)
                  {
                      size_t middle = size / 2;
                      median_result = static_cast<double>(times_result[middle - 1] + times_result[middle]) / 2.0;
                  }
                  else
                  {
                      median_result = static_cast<double>(times_result[size / 2]);
                  }

              }

              // #######
                int system_id = -1;
                if(configuration().get_library_name() == "our")
                {
                    // HAL system
                    system_id = 0;
                }
                else if(configuration().get_library_name() == "sortledton.4")
                {
                    // sortledton
                    system_id = 1;
                }
                else if(configuration().get_library_name() == "teseo.13")
                {
                    // teseo system
                    system_id = 2;
                }
                else
                {
                    // Livegraph
                    system_id = 3;
                }
                std::ofstream outfile1;
                std::cout<<configuration().num_threads(THREADS_WRITE)<<" "<<configuration().num_threads(THREADS_READ)<<" "<<system_id<<" "<<which_algo<<" "<<average_meps_throughput<<" "<<median_result<<"\n";
                outfile1.open(commonDir + "mixed_update_analytics_workload.txt", std::ios_base::app); // append instead of overwrite
                outfile1<<configuration().num_threads(THREADS_WRITE)<<" "<<configuration().num_threads(THREADS_READ)<<" "<<system_id<<" "<<which_algo<<" "<<average_meps_throughput<<" "<<median_result<<"\n";
                outfile1.close();
                //  outfile1<<system_id<<" "<<which_algo<<" "<<average_meps_throughput<<" "<<median_result<<"\n";
                //outfile1.close();

              cout << "Saving result" << endl;
              if (configuration().has_database()) result.save(configuration().db());
              cout << "Done saving" << endl;
            }
            else
            {
              LOG("[driver] Number of concurrent threads: " << configuration().num_threads(THREADS_WRITE));
              LOG("[driver] Aging2, path to the log of updates: " << configuration().get_update_log());
              Aging2Experiment experiment;
              experiment.set_library(impl_upd);
              experiment.set_log(configuration().get_update_log());
              experiment.set_parallelism_degree(configuration().num_threads(THREADS_WRITE));
              experiment.set_release_memory(configuration().get_aging_release_memory());
              experiment.set_report_progress(true);
              experiment.set_report_memory_footprint(configuration().get_aging_memfp_report());
              experiment.set_build_frequency(chrono::milliseconds{configuration().get_build_frequency()});
              experiment.set_max_weight(configuration().max_weight());
              experiment.set_measure_latency(configuration().measure_latency());
              experiment.set_num_reports_per_ops(configuration().get_num_recordings_per_ops());
              experiment.set_timeout(chrono::seconds{configuration().get_timeout_aging2()});
              experiment.set_measure_memfp(configuration().measure_memfp());
              experiment.set_memfp_physical(configuration().get_aging_memfp_physical());
              experiment.set_memfp_threshold(configuration().get_aging_memfp_threshold());
              experiment.set_cooloff(chrono::seconds{configuration().get_aging_cooloff_seconds()});
              experiment.library_name = configuration().m_library_name;
              auto result = experiment.execute();

              double num_op   = result.m_num_operations_total;
              double com_time = result.m_completion_time;

                std::cout<<"number of updates operations: "<<num_op<<std::endl;
              int system_id = -1;
              if(configuration().get_library_name() == "our")
              {
                    // HAL system
                    system_id = 0;
              }
              else if(configuration().get_library_name() == "sortledton.4")
              {
                    // sortledton
                    system_id = 1;
              }
              else if(configuration().get_library_name() == "teseo.13")
              {
                    // teseo system
                    system_id = 2;
              }
              else
              {
                    // Livegraph
                    system_id = 3;
              }

              // throughput average
              std::ofstream outfile1;
              outfile1.open(commonDir + "update_workload.txt", std::ios_base::app); // append instead of overwrite
              outfile1<<system_id<<" "<<(num_op/com_time)<<"\n";
              outfile1.close();

              if(configuration().m_aging_memfp)
              {
                 // if()
                 // impl_upd->on_thread_destroy(-1);
                  // throughput per second
                  std::ofstream outfile2;
                  outfile2.open(commonDir + "update_workload_per_second_operation.txt",std::ios_base::app); // append instead of overwrite
                  for (uint64_t i = 0, sz = result.m_progress.size(); i < sz; i++) {
                      outfile2 << system_id << " " << result.m_progress[i] << " " << result.m_progress.size() << " "<< num_op << "\n";
                  }
                  outfile2.close();

                  // memory consumption {1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100}
                  std::ofstream outfile3;
                  outfile3.open(commonDir + "update_workload_per_ten_second_space.txt",std::ios_base::app); // append instead of overwrite
                  std::cout << "memory footprint" << std::endl;
                  for (uint64_t i = 0, sz = result.m_memory_footprint.size(); i < sz; i++) {
                      // std::cout<<system_id<<" "<<result.m_memory_footprint[i].m_memory_driver<<" "<<result.m_memory_footprint[i].m_memory_process<<"\n";
                      outfile3 << system_id << " " << result.m_memory_footprint[i].m_memory_process << " "
                               << result.m_memory_footprint[i].m_memory_driver << "\n";
                  }
                  outfile3.close();
              }

              if (configuration().has_database()) result.save(configuration().db(), false); // false means store results in txt file
              random_vertex = result.get_random_vertex_id();

              if (configuration().validate_inserts() && impl_upd->can_be_validated())
              {
                LOG("[driver] Validation of updates requested, loading the original graph from: " << path_graph);
                auto stream = make_shared<graph::WeightedEdgeStream>(configuration().get_path_graph());
                num_validation_errors = validate_updates(impl_upd, stream);
              }
            }
        }
    }

    if(configuration().has_database()){
        vector<pair<string, string>> params;
        params.push_back(make_pair("num_validation_errors", to_string(num_validation_errors)));
        configuration().db()->store_parameters(params);
    }

    if(configuration().num_repetitions() > 0 && !configuration().is_mixed_workload())
    {

#if defined(HAVE_OPENMP)
        if(configuration().num_threads(ThreadsType::THREADS_READ) != 0 ){
            LOG("[driver] OpenMP, number of threads for the Graphalytics suite: " << configuration().num_threads(ThreadsType::THREADS_READ));
            omp_set_num_threads(configuration().num_threads(ThreadsType::THREADS_READ));
        }
#endif

        // run the graphalytics suite
        GraphalyticsAlgorithms properties { path_graph };

        if(properties.bfs.m_enabled == true && properties.sssp.m_enabled == false){
            LOG("[driver] Enabling SSSP with random weights, source vertex: " << random_vertex);
            properties.sssp.m_enabled = true;
            properties.sssp.m_source_vertex = random_vertex;
        }

        configuration().blacklist(properties);

        GraphalyticsSequential exp_seq { impl_ga, configuration().num_repetitions(), properties };

        if(configuration().validate_output()){
            LOG("[driver] Enabling validation mode");
            exp_seq.set_validate_output( configuration().get_validation_graph() );
            if(configuration().get_validation_graph() != path_graph){
                exp_seq.set_validate_remap_vertices( path_graph );
            }
        }
        exp_seq.execute();
        exp_seq.report(configuration().has_database());

    }

    LOG( "[driver] Done" );
}

int main(int argc, char* argv[]){

    int rc = 0;
    try {
        run_standalone(argc, argv);
    } catch(common::Error& e){
        cerr << e << endl;
        cerr << "Client terminating due to exception..." << endl;
        rc = 1;
    } catch(cxxopts::option_not_exists_exception& e){
        cerr << "ERROR: " << e.what() << "\n";
        rc = 1;
    } catch(cxxopts::option_requires_argument_exception& e){
        cerr << "ERROR: Invalid command line option, " << e.what() << "\n";
        rc = 1;
    }

    return rc;
}


