#ifndef _VRP_TABU_SEARCH_H_
#define _VRP_TABU_SEARCH_H_
#include <runners/TabuSearch.hh>
#include <iostream>
#include <string>
#include "data/route.h"
#include "data/prob_input.h"
#include "data/neighbor.h"
#include "helpers/vrp_state_manager.h"
#include "helpers/vrp_neighborhood_explorer.h"
#include "helpers/vrp_tabu_list_manager.h"

class InsMoveTabuSearch:
public TabuSearch<ProbInput, RoutePlan, InsMove> {
 public:
    InsMoveTabuSearch(const ProbInput &in,
                      VRPStateManager &e_sm,
                      InsMoveNeighborhoodExplorer &e_ne,
                      InsMoveTabuListManager &tlm,
                      std::string name, 
                      CLParser& cl,
                      AbstractTester<ProbInput, RoutePlan> &t,
                      std::ostream &log_os = std::cout):
        TabuSearch<ProbInput, RoutePlan, InsMove>(in, e_sm, e_ne,
                                                  tlm, name, cl, t),
        log(log_os) { }
    ~InsMoveTabuSearch() { }
    void StoreMove();
 private:
    std::ostream &log;
};

class InterSwapTabuSearch:
public TabuSearch<ProbInput, RoutePlan, InterSwap> {
 public:
    InterSwapTabuSearch(const ProbInput &in,
                      VRPStateManager &e_sm,
                      InterSwapNeighborhoodExplorer &e_ne,
                      InterSwapTabuListManager &tlm,
                      std::string name, 
                      CLParser& cl,
                      AbstractTester<ProbInput, RoutePlan> &t,
                      std::ostream &log_os = std::cout):
        TabuSearch<ProbInput, RoutePlan, InterSwap>(in, e_sm, e_ne,
                                                  tlm, name, cl, t),
        log(log_os) { }
    ~InterSwapTabuSearch() { }
    void StoreMove();
 private:
    std::ostream &log;
};

class IntraSwapTabuSearch:
public TabuSearch<ProbInput, RoutePlan, IntraSwap> {
 public:
    IntraSwapTabuSearch(const ProbInput &in,
                      VRPStateManager &e_sm,
                      IntraSwapNeighborhoodExplorer &e_ne,
                      IntraSwapTabuListManager &tlm,
                      std::string name, 
                      CLParser& cl,
                      AbstractTester<ProbInput, RoutePlan> &t,
                      std::ostream &log_os = std::cout):
        TabuSearch<ProbInput, RoutePlan, IntraSwap>(in, e_sm, e_ne,
                                                  tlm, name, cl, t),
        log(log_os) { }
    ~IntraSwapTabuSearch() { }
    void StoreMove();
 private:
    std::ostream &log;
};

void InsMoveTabuSearch::StoreMove() {
    log << "--" << std::endl;
    this->pm.Print(log);    // for debug, print tabu list
    log << "--" << std::endl;
    TabuSearch<ProbInput, RoutePlan, InsMove>::StoreMove();
}

void InterSwapTabuSearch::StoreMove() {
    log << "--" << std::endl;
    this->pm.Print(log);    // for debug, print tabu list
    log << "--" << std::endl;
    TabuSearch<ProbInput, RoutePlan, InterSwap>::StoreMove();
}

void IntraSwapTabuSearch::StoreMove() {
    log << "--" << std::endl;
    this->pm.Print(log);    // for debug, print tabu list
    log << "--" << std::endl;
    TabuSearch<ProbInput, RoutePlan, IntraSwap>::StoreMove();
}

#endif
