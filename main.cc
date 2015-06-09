#include <runners/TabuSearch.hh>
#include <observers/RunnerObserver.hh>
#include <observers/GeneralizedLocalSearchObserver.hh>
#include <solvers/SimpleLocalSearch.hh>
#include <solvers/GeneralizedLocalSearch.hh>
#include <testers/Tester.hh>
#include <utils/CLParser.hh>
#include <utils/Random.hh>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include "data/prob_input.h"
#include "data/neighbor.h"
#include "data/route.h"
#include "helpers/vrp_state_manager.h"
#include "helpers/vrp_neighborhood_explorer.h"
#include "helpers/vrp_tabu_list_manager.h"
#include "helpers/vrp_output_manager.h"
#include "helpers/vrp_tabu_search.h"
#include "solvers/vrp_token_ring_search.h"
#include "solvers/vrp_token_ring_observer.h"
#define RANDOM_MAX 0xffffffffUL

int main(int argc, char *argv[]) {
    CLParser cl(argc, argv);
    ValArgument<std::string> arg_input_file("file", "f", true, cl);
    ValArgument<int> arg_index("index", "i", true, cl);
    ValArgument<int> arg_cycle("cycle", "c", true, cl);
    ValArgument<int> arg_weight("weight", "w", true, cl);
    cl.MatchArgument(arg_input_file);
    cl.MatchArgument(arg_index);
    cl.MatchArgument(arg_cycle);
    cl.MatchArgument(arg_weight);

    std::string test_dir = "./test-cases/";
    std::string test_file = test_dir + arg_input_file.GetValue() + ".vrp";
    std::cout << test_file << std::endl;
    std::ifstream f(test_file.c_str());
    if (!f.is_open()) {
        std::cout << "No input case file." << std::endl;
        return 0;
    }
    ProbInput in(f);

    // helpers
    int weight = arg_weight.GetValue();
    VRPStateManager vrp_sm(in, weight);
    InsMoveNeighborhoodExplorer ins_ne(in, vrp_sm, weight);
    InterSwapNeighborhoodExplorer intersw_ne(in, vrp_sm, weight);
    IntraSwapNeighborhoodExplorer intrasw_ne(in, vrp_sm, weight);
    InsMoveTabuListManager ins_tlm(4);
    InterSwapTabuListManager intersw_tlm;
    IntraSwapTabuListManager intrasw_tlm;
    VRPOutputManager vrp_om(in, "VRPOutputManager");

    // testers
    Tester<ProbInput, ProbOutput, RoutePlan> tester(in, vrp_sm, vrp_om);
    // observer
	std::string log_head("./logs/");
	log_head += arg_input_file.GetValue() + "-";
    // std::ofstream ins_log(log_head + "ins.log");
    // std::ofstream ins_tabu(log_head + "ins.tabu");
    // std::ofstream inter_log(log_head + "inter.log");
    // std::ofstream inter_tabu(log_head + "inter.tabu");
    // std::ofstream intra_log(log_head + "intra.log");
    // std::ofstream intra_tabu(log_head + "intra.tabu");
    // RunnerObserver<ProbInput, RoutePlan, InsMove> ins_ro(2, 2,
    //                                                      ins_log,
    //                                                      ins_log);
    // RunnerObserver<ProbInput, RoutePlan, InterSwap> inter_ro(2, 2,
    //                                                          inter_log,
    //                                                          inter_log);
    // RunnerObserver<ProbInput, RoutePlan, IntraSwap> intra_ro(2, 2,
    //                                                          intra_log,
    //                                                          intra_log);
    // runners
    InsMoveTabuSearch ts_ins(in, vrp_sm,
                             ins_ne, ins_tlm,
                             "InsMoveTabuSearch",
                             cl, tester, false);
    InterSwapTabuSearch ts_intersw(in, vrp_sm,
                             intersw_ne, intersw_tlm,
                             "InterSwapTabuSearch",
                             cl, tester, false);
    IntraSwapTabuSearch ts_intrasw(in, vrp_sm,
                             intrasw_ne, intrasw_tlm,
                             "IntraSwapTabuSearch",
                             cl, tester, false);

    // solvers
    TokenRingSearch token_ring_solver(in, vrp_sm, vrp_om, "TokenRing", "./", cl);
    // std::ofstream token_ring_f(log_head + "token_ring.log");
    // TokenRingObserver tr_observer(token_ring_f);


    // Token Ring Search
    int max_iteration = 1000;
    ts_ins.SetMaxIteration(max_iteration);
    ts_intersw.SetMaxIteration(max_iteration);
    ts_intrasw.SetMaxIteration(max_iteration);
    // token_ring_solver.AttachObserver(tr_observer);
    // ts_ins.AttachObserver(ins_ro);
    // ts_intersw.AttachObserver(inter_ro);
    // ts_intrasw.AttachObserver(intra_ro);
    token_ring_solver.SetInitTrials(2);
    token_ring_solver.AddRunner(ts_ins);
    token_ring_solver.AddRunner(ts_intersw);
    token_ring_solver.AddRunner(ts_intrasw);
    int cycle = arg_cycle.GetValue();
    int index = arg_index.GetValue();
    Random::Seed((unsigned long)(time(NULL) % RANDOM_MAX + index));
    for (int i = 0; i < cycle; ++i) {
        token_ring_solver.Solve();
        std::ostringstream os_file;
        os_file << "./300/" << arg_input_file.GetValue()
                << arg_index.GetValue() << ".out." << i;
        std::ofstream out_f(os_file.str().c_str());
        out_f << token_ring_solver.GetOutput() << std::endl;
    }

    // int best_cost = vrp_sm.CostFunction(token_ring_solver.GetOutput());
    // test_file = test_file.substr(0, test_file.size() - 3) + "out";
    // std::ofstream out_f(test_file.c_str());
    // out_f << token_ring_solver.GetOutput() << std::endl;
    // out_f << "Best cost: " << best_cost << std::endl;

    return 0;
}
