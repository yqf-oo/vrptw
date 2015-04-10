#include <runners/TabuSearch.hh>
#include <observers/RunnerObserver.hh>
#include <solvers/SimpleLocalSearch.hh>
#include <solvers/GeneralizedLocalSearch.hh>
#include <testers/Tester.hh>
#include <utils/CLParser.hh>
#include <fstream>
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

int main(int argc, char *argv[]) {
    CLParser cl(argc, argv);
    ValArgument<std::string> arg_input_file("file", "f", true, cl);
    cl.MatchArgument(arg_input_file);
    // test instance file input
    std::string test_dir = "./test-cases/";
    std::string test_file = test_dir + arg_input_file.GetValue();
    std::cout << test_file << std::endl;
    std::ifstream f(test_file.c_str());
    if (!f.is_open()) {
        std::cout << "No input case file." << std::endl;
        return 0;
    }
    ProbInput in(f);

    // helpers
    unsigned min_tenture = 0, max_tenture = 0;
    VRPStateManager vrp_sm(in);
    InsMoveNeighborhoodExplorer ins_ne(in, vrp_sm);
    InterSwapNeighborhoodExplorer intersw_ne(in, vrp_sm);
    IntraSwapNeighborhoodExplorer intrasw_ne(in, vrp_sm);
    InsMoveTabuListManager ins_tlm(min_tenture, max_tenture);
    InterSwapTabuListManager intersw_tlm(min_tenture, max_tenture);
    IntraSwapTabuListManager intrasw_tlm(min_tenture, max_tenture);
    VRPOutputManager vrp_om(in, "VRPOutputManager");

    // testers
    Tester<ProbInput, ProbOutput, RoutePlan> tester(in, vrp_sm, vrp_om);
    // observer
    std::ofstream ins_log("./logs/ins.log");
    std::ofstream ins_tabu("./logs/ins.tabu");
    std::ofstream inter_log("./logs/inter.log");
    std::ofstream inter_tabu("./logs/inter.tabu");
    std::ofstream intra_log("./logs/intra.log");
    std::ofstream intra_tabu("./logs/intra.tabu");
    RunnerObserver<ProbInput, RoutePlan, InsMove> ins_ro(2, 2,
                                                         ins_log,
                                                         ins_log);
    RunnerObserver<ProbInput, RoutePlan, InterSwap> inter_ro(2, 2,
                                                             inter_log,
                                                             inter_log);
    RunnerObserver<ProbInput, RoutePlan, IntraSwap> intra_ro(2, 2,
                                                             intra_log,
                                                             intra_log);
    // runners
    InsMoveTabuSearch ts_ins(in, vrp_sm,
                             ins_ne, ins_tlm,
                             "InsMoveTabuSearch",
                             cl, tester, ins_tabu);
    InterSwapTabuSearch ts_intersw(in, vrp_sm,
                             intersw_ne, intersw_tlm,
                             "InterSwapTabuSearch",
                             cl, tester, inter_tabu);
    IntraSwapTabuSearch ts_intrasw(in, vrp_sm,
                             intrasw_ne, intrasw_tlm,
                             "IntraSwapTabuSearch",
                             cl, tester, intra_tabu);
    // TabuSearch<ProbInput, RoutePlan, InsMove> ts_ins(in, vrp_sm,
    //                                                  ins_ne, ins_tlm,
    //                                                  "InsMoveTabuSearch",
    //                                                  cl, tester);
    // TabuSearch<ProbInput, RoutePlan, InterSwap> ts_intersw(in, vrp_sm,
    //                                                        intersw_ne,
    //                                                        intersw_tlm,
    //                                                        "InterSwapTS",
    //                                                         cl, tester);
    // TabuSearch<ProbInput, RoutePlan, IntraSwap> ts_intrasw(in, vrp_sm,
    //                                                        intrasw_ne,
    //                                                        intrasw_tlm,
    //                                                        "IntraSwapTS",
    //                                                        cl, tester);

    // solvers
    SimpleLocalSearch<ProbInput, ProbOutput, RoutePlan> sls(in, vrp_sm, vrp_om,
                                                            "SimLocalSearch");
    TokenRingSearch token_ring_solver(in, vrp_sm, vrp_om, "VRPTokenRing", cl);
    std::ofstream token_ring_f("./logs/token_ring.log");
    TokenRingObserver tr_observer(token_ring_f);

    // ts_ins.SetMaxIteration(1000);
    // ts_intrasw.AttachObserver(intra_ro);
    // sls.SetRunner(ts_intrasw);
    // sls.Solve();

    token_ring_solver.AttachObserver(tr_observer);
    token_ring_solver.AddRunner(ts_ins);
    token_ring_solver.AddRunner(ts_intersw);
    token_ring_solver.AddRunner(ts_intrasw);
    token_ring_solver.Solve();

    int best_cost = vrp_sm.CostFunction(token_ring_solver.GetOutput());
    test_file = test_file.substr(0, test_file.size() - 3) + "out";
    std::ofstream out_f(test_file.c_str());
    out_f << token_ring_solver.GetOutput() << std::endl;
    out_f << "Best cost: " << best_cost << std::endl;
    // // test cost computation
    // RoutePlan test_state(in);
    // test_file = test_file.substr(0, test_file.size() - 3) + "sol";
    // std::fstream state_f(test_file.c_str());
    // state_f >> test_state;
    // std::cout << test_file << std::endl << test_state << std::endl;
    // vrp_sm.UpdateTimeTable(test_state);
    // std::cout << vrp_sm.CostFunction(test_state) << std::endl;
    // for (unsigned i = 0; i < test_state.num_routes(); ++i) {
    //     std::cout << "Route " << i << ":";
    //     for (unsigned j = 0; j <= test_state[i].size(); ++j)
    //         std::cout << " " << test_state(i, j);
    //     std::cout << std::endl;
    // }

    // // test delta cost computation
    // vrp_sm.UpdateTimeTable(test_state);
    // std::cout << "Move: " << mv << std::endl;
    // int delta = intrasw_ne.DeltaCostFunction(test_state, mv);
    // intrasw_ne.MakeMove(test_state, mv);
    // std::ofstream out("test-state");
    // out << test_state;
    // int cost_aft = vrp_sm.CostFunction(test_state);
    // std::cout << cost_bef << ", " << cost_aft << std::endl;
    // std::cout<< "ne: " << delta << ", "
    //          << cost_aft - cost_bef << std::endl;

    return 0;
}
