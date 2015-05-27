#include <runners/TabuSearch.hh>
#include <observers/RunnerObserver.hh>
#include <observers/GeneralizedLocalSearchObserver.hh>
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
    // ValArgument<int> max_rounds("mr", "mr", true, cl);
    // ValArgument<int> max_idle_rounds("mir", "mir", true, cl);
    // ValArgument<int> max_idle_iteration("mii", "mii", true, cl);
    cl.MatchArgument(arg_input_file);
    // cl.MatchArgument(max_rounds);
    // cl.MatchArgument(max_idle_rounds);
    // cl.MatchArgument(max_idle_iteration);
    // test instance file input
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
	std::string log_head("./logs/");
	log_head += arg_input_file.GetValue() + "-";
    std::ofstream ins_log(log_head + "ins.log");
    std::ofstream ins_tabu(log_head + "ins.tabu");
    std::ofstream inter_log(log_head + "inter.log");
    std::ofstream inter_tabu(log_head + "inter.tabu");
    std::ofstream intra_log(log_head + "intra.log");
    std::ofstream intra_tabu(log_head + "intra.tabu");
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
                             cl, tester, false, ins_tabu);
    InterSwapTabuSearch ts_intersw(in, vrp_sm,
                             intersw_ne, intersw_tlm,
                             "InterSwapTabuSearch",
                             cl, tester, false, inter_tabu);
    IntraSwapTabuSearch ts_intrasw(in, vrp_sm,
                             intrasw_ne, intrasw_tlm,
                             "IntraSwapTabuSearch",
                             cl, tester, false, intra_tabu);
    // set arguments
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
    TokenRingSearch token_ring_solver(in, vrp_sm, vrp_om, "TokenRing", cl);
    std::ofstream token_ring_f(log_head + "token_ring.log");
    TokenRingObserver tr_observer(token_ring_f);

    // // Simple Local Search
    // ts_ins.AttachObserver(ins_ro);
    // sls.SetInitTrials(1);
    // sls.SetRunner(ts_ins);
    // sls.Solve();

    // Token Ring Search
    int max_iteration = 1000;
    ts_ins.SetMaxIteration(max_iteration);
    ts_intersw.SetMaxIteration(max_iteration);
    ts_intrasw.SetMaxIteration(max_iteration);
    token_ring_solver.AttachObserver(tr_observer);
    ts_ins.AttachObserver(ins_ro);
    ts_intersw.AttachObserver(inter_ro);
    ts_intrasw.AttachObserver(intra_ro);
    token_ring_solver.SetInitTrials(2);
    token_ring_solver.AddRunner(ts_ins);
    token_ring_solver.AddRunner(ts_intersw);
    token_ring_solver.AddRunner(ts_intrasw);
    token_ring_solver.MultiStartSolve(100);


    // // General Local Search
    // GeneralizedLocalSearch<ProbInput, ProbOutput, RoutePlan> gls(in, vrp_sm,
    //                                                              vrp_om,
    //                                                              "TokenRing",
    //                                                              cl);
    // GeneralizedLocalSearchObserver<ProbInput, ProbOutput, RoutePlan> gls_ob(2, 2,
    //                                                                         token_ring_f,
    //                                                                         token_ring_f);
    // gls.AttachObserver(gls_ob);
    // gls.AddRunner(ts_ins);
    // gls.AddRunner(ts_intersw);
    // gls.AddRunner(ts_intrasw);
    // gls.GeneralSolve();

    // // // Move Test
    // RoutePlan test_state(in);
    // test_file = test_file.substr(0, test_file.size() - 3) + "sol";
    // std::ifstream state_f(test_file.c_str());
    // state_f >> test_state;
    // vrp_sm.UpdateTimeTable(test_state);
    // std::cout << vrp_sm.CostFunction(test_state) << std::endl;
    // std::ofstream state_out_f("./logs/state.log");
    // state_out_f << test_state << std::endl;
    // IntraSwap mv;
    // intrasw_ne.FirstMove(test_state, mv);
    // state_out_f << mv << std::endl;
    // while (intrasw_ne.NextMove(test_state, mv))
    //     state_out_f << mv << std::endl;

    // Output Best State
    // for (int i = 0; i < 20; ++i) {
    //     sls.Solve();
    int best_cost = vrp_sm.CostFunction(token_ring_solver.GetOutput());
    test_file = test_file.substr(0, test_file.size() - 3) + "out";
    std::ofstream out_f(test_file.c_str());
    out_f << token_ring_solver.GetOutput() << std::endl;
    out_f << "Best cost: " << best_cost << std::endl;
	out_f << "Test " << token_ring_solver.get_num_trials() << " trials." << std::endl;
	std::cout << "Test " << token_ring_solver.get_num_trials() << " trials." << std::endl;
    // }

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
