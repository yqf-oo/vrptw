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
                      bool flag,
                      std::ostream &log_os = std::cout):
        TabuSearch<ProbInput, RoutePlan, InsMove>(in, e_sm, e_ne,
                                                  tlm, name, cl, t),
        have_log(flag), log(log_os) { }
    ~InsMoveTabuSearch() { }
    void StoreMove();
 private:
    bool have_log;
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
					    bool flag,
                        std::ostream &log_os = std::cout):
        TabuSearch<ProbInput, RoutePlan, InterSwap>(in, e_sm, e_ne,
                                                  tlm, name, cl, t),
        have_log(flag), log(log_os) { }
    ~InterSwapTabuSearch() { }
    void StoreMove();
 private:
	bool have_log;
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
                        bool flag,
                        std::ostream &log_os = std::cout):
        TabuSearch<ProbInput, RoutePlan, IntraSwap>(in, e_sm, e_ne,
                                                  tlm, name, cl, t),
        have_log(flag), log(log_os) { }
    ~IntraSwapTabuSearch() { }
    void StoreMove();
 private:
	bool have_log;
    std::ostream &log;
};

void InsMoveTabuSearch::StoreMove() {
    if (have_log) {
        log << "--" << std::endl;
        log << this->current_state << std::endl
            << this->current_move << std::endl
            << this->current_move_cost << std::endl;
        log << "--" << std::endl;
    }
    if (this->observer != NULL)
        this->observer->NotifyStoreMove(*this);
    this->pm.InsertMove(this->current_state, this->current_move, this->current_move_cost,
                        this->current_state_cost, this->best_state_cost);
    bool cur_vio = this->current_state.get_vio() > 0 ? true : false;
    bool best_vio = this->best_state.get_vio() > 0 ? true: false;
    if (!(cur_vio ^ best_vio)) {
        if (LessOrEqualThan(this->current_state_cost,this->best_state_cost)) {
            // same cost states are accepted as best for diversification
            if (LessThan(this->current_state_cost,this->best_state_cost)) {
                if (this->observer != NULL)
                    this->observer->NotifyNewBest(*this);
                this->iteration_of_best = this->number_of_iterations;
                this->best_state_cost = this->current_state_cost;
            }
            this->best_state = this->current_state;
        }
    } else if (!cur_vio && best_vio) {
        if (this->observer != NULL)
            this->observer->NotifyNewBest(*this);
        this->iteration_of_best = this->number_of_iterations;
        this->best_state_cost = this->current_state_cost;
        this->best_state = this->current_state;
    }
    // TabuSearch<ProbInput, RoutePlan, InterSwap>::StoreMove();
}

void InterSwapTabuSearch::StoreMove() {
	if (have_log) {
		log << "--" << std::endl;
		this->pm.Print(log);    // for debug, print tabu list
		log << "--" << std::endl;
	}
    if (this->observer != NULL)
        this->observer->NotifyStoreMove(*this);
    this->pm.InsertMove(this->current_state, this->current_move, this->current_move_cost,
                        this->current_state_cost, this->best_state_cost);
    bool cur_vio = this->current_state.get_vio() > 0 ? true : false;
    bool best_vio = this->best_state.get_vio() > 0 ? true: false;
    if (!(cur_vio ^ best_vio)) {
        if (LessOrEqualThan(this->current_state_cost,this->best_state_cost)) {
            // same cost states are accepted as best for diversification
            if (LessThan(this->current_state_cost,this->best_state_cost)) {
                if (this->observer != NULL)
                    this->observer->NotifyNewBest(*this);
                this->iteration_of_best = this->number_of_iterations;
                this->best_state_cost = this->current_state_cost;
            }
            this->best_state = this->current_state;
        }
    } else if (!cur_vio && best_vio) {
        if (this->observer != NULL)
            this->observer->NotifyNewBest(*this);
        this->iteration_of_best = this->number_of_iterations;
        this->best_state_cost = this->current_state_cost;
        this->best_state = this->current_state;
    }
    // TabuSearch<ProbInput, RoutePlan, InterSwap>::StoreMove();
}

void IntraSwapTabuSearch::StoreMove() {
	if (have_log) {
		log << "--" << std::endl;
		this->pm.Print(log);    // for debug, print tabu list
		log << "--" << std::endl;
	}
    if (this->observer != NULL)
        this->observer->NotifyStoreMove(*this);
    this->pm.InsertMove(this->current_state, this->current_move, this->current_move_cost,
                        this->current_state_cost, this->best_state_cost);
    bool cur_vio = this->current_state.get_vio() > 0 ? true : false;
    bool best_vio = this->best_state.get_vio() > 0 ? true: false;
    if (!(cur_vio ^ best_vio)) {
        if (LessOrEqualThan(this->current_state_cost,this->best_state_cost)) {
            // same cost states are accepted as best for diversification
            if (LessThan(this->current_state_cost,this->best_state_cost)) {
                if (this->observer != NULL)
                    this->observer->NotifyNewBest(*this);
                this->iteration_of_best = this->number_of_iterations;
                this->best_state_cost = this->current_state_cost;
            }
            this->best_state = this->current_state;
        }
    } else if (!cur_vio && best_vio) {
        if (this->observer != NULL)
            this->observer->NotifyNewBest(*this);
        this->iteration_of_best = this->number_of_iterations;
        this->best_state_cost = this->current_state_cost;
        this->best_state = this->current_state;
    }
    // TabuSearch<ProbInput, RoutePlan, InterSwap>::StoreMove();
}

#endif
