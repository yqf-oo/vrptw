#include <runners/TabuSearch.hh>
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
    // runners
    TabuSearch<ProbInput, RoutePlan, InsMove> ts_ins(in, vrp_sm, ins_ne, ins_tlm,
                                                     "InsMoveTabuSearch",
                                                     cl, tester);
    // TabuSearch<ProbInput, RoutePlan, InterSwap> ts_intersw(in, vrp_sm,
    //                                                        intersw_ne,
    //                                                        intersw_tlm,
    //                                                         "InterSwapTabuSearch",
    //                                                         cl, tester);
    // TabuSearch<ProbInput, RoutePlan, IntraSwap> ts_intrasw(in, vrp_sm, intrasw_ne,
    //                                                    intrasw_tlm,
    //                                                    "IntraSwapTabuSearch",
    //                                                    cl, tester);

    // solvers
    SimpleLocalSearch<ProbInput, ProbOutput, RoutePlan> sls(in, vrp_sm, vrp_om,
                                                            "InsSimpleLocalSearch");
    GeneralizedLocalSearch<ProbInput, ProbOutput, RoutePlan> gls(in, vrp_sm, vrp_om,
                                                                 "TokenRingLocalSearch");

    sls.SetRunner(ts_ins);
    sls.Solve();
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
    return 0;
}
