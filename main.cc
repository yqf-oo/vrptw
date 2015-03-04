#include <runners/TabuSearch.hh>
#include <solvers/SimpleLocalSearch>
#include <solvers/GeneralizedLocalSearch>
#include <testers/Tester.hh>
#include <utils/CLParser.hh>
#include <iostream>
#include "data/prob_input.h"
#include "data/move.h"
#include "data/route.h"
#include "helpers/vrp_state_manager.h"
#include "helpers/vrp_neighbrohood_explorer.h"
#include "helpers/vrp_tabu_list_manager.h"
#include "helpers/vrp_output_manager.h"

int main(int argc, char *argv[]) {
    CLParser cl(argc, argv);
    // test instance file input
    ifstream f("input-case");
    ProbInput in(f);

    // helpers
    VRPStateManager vrp_sm(in);
    InsMoveNeighborhoodExplorer ins_ne(in, vrp_sm);
    InterSwapNeighborhoodExplorer intersw_ne(in, vrp_sm);
    IntraSwapNeighborHoodExplorer intrasw_ne(in, vrp_sm);
    InsMoveTabuListManager ins_tlm(min_tenture, max_tenture);
    InterSwapTabuListManager intersw_tlm(min_tenture, max_tenture);
    IntraSwapTabuListManager intrasw_tlm(min_tenture, max_tenture);
    VRPOutputManager vrp_om(in);

    // testers
    Tester<ProbInput, RoutePlan> tester(in, vrp_sm, vrp_om);
    // runners
    TabuSearch<ProbInput, RoutePlan, InsMove> ts_ins(in, vrp_sm, ins_ne, ins_tlm
                                                     "InsertTabuSearch",
                                                     cl, tester);
    TabuSearch<ProbInput, RoutePlan, InterSwap> ts_intersw(in, vrp_sm,
                                                           intersw_ne,
                                                           intersw_tlm,
                                                            "InterSwapTabuSearch",
                                                            cl, tester);
    TabuSearch<ProbInput, RoutePlan, IntraSwap> ts_ins(in, vrp_sm, intrasw_ne,
                                                       intrasw_tlm,
                                                       "IntraSwapTabuSearch",
                                                       cl, tester);

    // solvers
    SimpleLocalSearch<ProbInput, ProbOutput, RoutePlan> sls(in, vrp_sm, vrp_om,
                                                            "InsSimpleLocalSearch");
    GeneralizedLocalSearch<ProbInput, ProbOutput, RoutePlan> gls(in, vrp_sm, vrp_om,
                                                                 "TokenRingLocalSearch");

    sls.SetRunner(ts_ins);
    sls.Solve();
}
