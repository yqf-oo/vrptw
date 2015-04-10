#include "solvers/vrp_token_ring_search.h"
#include <utils/Types.hh>
#include <stdexcept>
#include "solvers/vrp_token_ring_observer.h"

TokenRingSearch::TokenRingSearch(const ProbInput &in,
                                 VRPStateManager &e_sm,
                                 VRPOutputManager &e_om,
                                 std::string name):
    AbstractLocalSearch<ProbInput, ProbOutput, RoutePlan>(in, e_sm,
                                                          e_om, name),
    current_runner_(0), round_(0), idle_rounds_(0),
    max_rounds_(100), max_idle_rounds_(1), num_trails_(0),
    token_ring_arguments("tr_" + name, "tr_" + name, false),
    arg_max_idle_rounds("max_idle_rounds", "mir", false),
    arg_max_rounds("max_rounds", "mr", false),
    arg_timeout("timeout", "to", false, 0.0) {
    token_ring_arguments.AddArgument(arg_max_rounds);
    token_ring_arguments.AddArgument(arg_max_idle_rounds);
    token_ring_arguments.AddArgument(arg_timeout);
}

TokenRingSearch::TokenRingSearch(const ProbInput &in,
                                 VRPStateManager &e_sm,
                                 VRPOutputManager &e_om,
                                 std::string name,
                                 CLParser &cl):
    AbstractLocalSearch<ProbInput, ProbOutput, RoutePlan>(in, e_sm,
                                                          e_om, name),
    current_runner_(0), round_(0), idle_rounds_(0),
    max_rounds_(100), max_idle_rounds_(1), num_trails_(0),
    token_ring_arguments("tr_" + name, "tr_" + name, false),
    arg_max_idle_rounds("max_idle_rounds", "mir", false),
    arg_max_rounds("max_rounds", "mr", false),
    arg_timeout("timeout", "to", false, 0.0) {
    token_ring_arguments.AddArgument(arg_max_rounds);
    token_ring_arguments.AddArgument(arg_max_idle_rounds);
    token_ring_arguments.AddArgument(arg_timeout);
    cl.MatchArgument(token_ring_arguments);
    if (token_ring_arguments.IsSet()) {
        if (arg_max_rounds.IsSet())
            max_rounds_ = arg_max_rounds.GetValue();
        if (arg_max_idle_rounds.IsSet())
            max_idle_rounds_ = arg_max_idle_rounds.GetValue();
        if (arg_timeout.IsSet())
            this->SetTimeout(arg_timeout.GetValue());
    }
}

void TokenRingSearch::Solve() {
    Run();
}

void TokenRingSearch::MultiStartSolve(unsigned trials) {
    RoutePlan global_best_state(this->in);
    int global_best_state_cost = 0;
    bool timeout_expired = false;

    for (unsigned t = 0; t < trials; ++t) {
        if (observer != NULL) observer->NotifyRestart(*this, t);
        this->Run();
        if (t == 0 || LessThan(this->best_state_cost, global_best_state_cost)) {
            global_best_state = this->best_state;
            global_best_state_cost = this->best_state_cost;
            if (LowerBoundReached(global_best_state_cost)) break;
        }
#ifdef _VRP_HAVE_PTHREAD_
        if (this->timeout_set) {
            if (this->current_timeout <= 0.0) {
                timeout_expired = true;
                this->current_timeout = 0.0;
            }
        }
#endif
        if (timeout_expired) break;
        ++num_trails_;
    }
    this->best_state = global_best_state;
    this->best_state_cost = global_best_state_cost;
}

void TokenRingSearch::InitializeSearch() {
    if (max_idle_rounds_ == 0)
        throw std::logic_error("Max idle round should be greater than 0");
    if (max_idle_rounds_ == 0)
        throw std::logic_error("Max idle round should be greater than 0");
    if (!p_runners.size())
        throw std::logic_error("No runner in " + this->name);
}

void TokenRingSearch::Run() {
    InitializeSearch();
    this->FindInitialState();
    bool timeout_expired = false;
    bool lower_bound_reached = false;
    chrono.Reset();
    chrono.Start();
    this->best_state = this->current_state;
    this->best_state_cost = this->current_state_cost;
    do {
        ++round_;
        ++idle_rounds_;
        for (RunnerType *p_r : p_runners) {
            p_r->SetState(this->current_state, this->current_state_cost);
            if (observer != NULL) observer->NotifyRunnerStart(*this);
            timeout_expired = this->LetGo(*p_r);
            if (observer != NULL) observer->NotifyRunnerStop(*this);
            this->current_state = p_runners[current_runner_]->GetState();
            this->current_state_cost = p_r->GetStateCost();
            if (LessThan(this->current_state_cost, this->best_state_cost)) {
                idle_rounds_ = 0;
                this->best_state = this->current_state;
                this->best_state_cost = this->current_state_cost;
                lower_bound_reached = LowerBoundReached(this->best_state_cost);
            }
            current_runner_ = (current_runner_ + 1) % p_runners.size();
            if (observer != NULL) observer->NotifyRound(*this);
            if (lower_bound_reached || timeout_expired) break;
        }
    }while(round_ < max_rounds_ || idle_rounds_ < max_idle_rounds_
           || lower_bound_reached || timeout_expired);
    chrono.Stop();
}

bool TokenRingSearch::LowerBoundReached(int state_cost) const {
    return this->sm.LowerBoundReached(state_cost);
}

void TokenRingSearch::AddRunner(RunnerType &r) {
    p_runners.push_back(&r);
}

void TokenRingSearch::RemoveRunner(RunnerType &r) {
    typename std::vector<RunnerType*>::const_iterator it;
    for (it = p_runners.begin(); it != p_runners.end(); ++it) {
        if (*it == &r)
            break;
    }
    if (it != p_runners.end())
        p_runners.erase(it);
    return;
}

void TokenRingSearch::ReadParameters(std::istream &is, std::ostream &os ) {
    os << "Token Ring Search: " << this->name << "params" << std::endl;
    for (RunnerType *p_r : p_runners) {
        p_r->ReadParameters(is, os);
    }
}

void TokenRingSearch::Print(std::ostream &os) const {
    os << "Token Ring Search: " << this->name << std::endl;
    for (const RunnerType *p_r : p_runners) {
        p_r->Print(os);
    }
    if (!p_runners.size())
        os << "No runner attacked." << std::endl;
}