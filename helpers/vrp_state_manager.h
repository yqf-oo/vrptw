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
    ~VRPStateManager() { }
    void RandomState(RoutePlan&);
    bool CheckConsistency(const RoutePlan&) const { return true; }
    unsigned CostFunction(const RoutePlan&) const;
    unsigned Objective(const RoutePlan&) const;
    unsigned Violations(const RoutePlan&) const;
 private:
    void ResetState(RoutePlan&);
    void UpdateTimeTable(RoutePlan&);
    unsigned ComputeDateViolationCost(const RoutePlan&, int) const;    // s1
    unsigned ComputeTimeViolationCost(const RoutePlan&, int) const;    // s2
    unsigned ComputeOptOrderCost(const RoutePlan&, int) const;         // s3
    unsigned ComputeTranportationCost(const RoutePlan&) const;         // s4
    unsigned ComputeCapExceededCost(const RoutePlan&, int) const;      // h1
    unsigned ComputeLateReturnCost(const RoutePlan&, int) const;       // h2
};

#endif
