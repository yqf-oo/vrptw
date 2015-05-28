#ifndef _VRP_TOKEN_RING_SEARCH_H_
#define _VRP_TOKEN_RING_SEARCH_H_
#include <solvers/AbstractLocalSearch.hh>
#include <runners/Runner.hh>
#include <utils/CLParser.hh>
#include <utils/Chronometer.hh>
#include <iostream>
#include <string>
#include <vector>
#include "data/route.h"
#include "data/prob_input.h"
#include "helpers/vrp_state_manager.h"
#include "helpers/vrp_output_manager.h"

#define _VRP_HAVE_PTHREAD_

class TokenRingObserver;
class TokenRingSearch:
public AbstractLocalSearch<ProbInput, ProbOutput, RoutePlan> {
    friend class TokenRingObserver;

 public:
    typedef Runner<ProbInput, RoutePlan> RunnerType;
    TokenRingSearch(const ProbInput &in,
                    VRPStateManager &e_sm,
                    VRPOutputManager &e_om,
                    std::string name);
    TokenRingSearch(const ProbInput& in,
                    VRPStateManager &e_sm,
                    VRPOutputManager &e_om,
                    std::string name,
                    CLParser &cl);
    virtual ~TokenRingSearch() { }
    void Solve();
    void MultiStartSolve(unsigned);
    void AttachObserver(TokenRingObserver &tro) { observer = &tro; }
    void AddRunner(RunnerType&);
    void RemoveRunner(RunnerType&);
    void Print(std::ostream &os = std::cout) const;
    void ReadParameters(std::istream& is = std::cin,
                        std::ostream& os = std::cout);
    int round() const { return round_; }
    int idle_rounds() const { return idle_rounds_; }
    void set_max_rounds(unsigned r) { max_rounds_ = r; }
    void set_max_idle_rounds(unsigned r) { max_idle_rounds_ = r; }
	int get_num_trials() const { return num_trials_; }

 private:
    void Run();
    void InitializeSearch();
    bool LowerBoundReached(int state_cost) const;
    std::vector<RunnerType*> p_runners;
    unsigned current_runner_;
    int round_;
    int idle_rounds_;
    int max_rounds_, max_idle_rounds_, max_idle_trials_;
    int num_trials_;
	int idle_trials_;
    ArgumentGroup token_ring_arguments;
    ValArgument<int> arg_max_rounds;
    ValArgument<int> arg_max_idle_rounds;
    ValArgument<int> arg_max_idle_trials;
    ValArgument<double> arg_timeout;
    TokenRingObserver *observer;
    Chronometer chrono;
};

#endif
