#ifndef _VRP_STATE_MANAGER_H_
#define _VRP_STATE_MANAGER_H_
#include <helpers/StateManager.hh>
#include <iostream>
#include <string>
#include <vector>
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
    int operator(unsigned r, unsigned o) const { return timetable[r][o]; }
    int& operator(unsigned r, unsigned o ) { return timetable[r][o]; }
    const std::vector<int>& operator[](unsigned i) const { return timetable[i]; }
    std::vector<int>& operator[](unsigned i) { return timetable[i]; }
 private:
    void ResetState(RoutePlan&);
    void UpdateTimeTable(RoutePlan&);
    unsigned ComputeDateViolationCost(const RoutePlan&, int) const;    // s1
    unsigned ComputeTimeViolationCost(const RoutePlan&, int) const;    // s2
    unsigned ComputeOptOrderCost(const RoutePlan&, int) const;         // s3
    unsigned ComputeTranportationCost(const RoutePlan&) const;         // s4
    unsigned ComputeCapExceededCost(const RoutePlan&, int) const;      // h1
    unsigned ComputeLateReturnCost(const RoutePlan&, int) const;       // h2
    std::vector<std::vector<int>> timetable;
};

#endif
