#ifndef _VRP_STATE_MANAGER_H_
#define _VRP_STATE_MANAGER_H_
#include <helpers/StateManager.hh>
#include <iostream>
#include <string>
#include "data/prob_input.h"
#include "data/route.h"

class VRPStateManager: public StateManager<ProbInput, RoutePlan> {
 public:
    VRPStateManager(const ProbInput &pi):
        StateManager<ProbInput, RoutePlan>(pi, "VRPStateManager") { }
    ~VRPStateManager() { ClearCostComponent(); }
    void RandomState(RoutePlan&);
    bool CheckConsistency(const RoutePlan&) const { return true; }
    unsigned CostFunction(const RoutePlan&) const;
    unsigned Objective(const RoutePlan&) const;
    unsigned Violations(const RoutePlan&) const;
 private:
    void ResetState(RoutePlan&);
    void UpdateTimeTable(RoutePlan&);
    int ComputeDateViolationCost(const RoutePlan&, int);
    int ComputeTimeViolationCost(const RoutePlan&, int);
    int ComputeOptOrderCost(const RoutePlan&, int);
};

#endif
