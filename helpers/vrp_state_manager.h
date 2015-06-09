#ifndef _VRP_STATE_MANAGER_H_
#define _VRP_STATE_MANAGER_H_
#include <helpers/StateManager.hh>
#include <iostream>
#include <string>
#include <vector>
#include "data/route.h"
#include "data/prob_input.h"

// #define _STATE_DEBUG_H_
#define _INIT_H_

class VRPStateManager: public StateManager<ProbInput, RoutePlan> {
 public:
    explicit VRPStateManager(const ProbInput &pi, int w):
        StateManager<ProbInput, RoutePlan>(pi, "VRPStateManager"),
        num_order_late_return(0), cap_vio_cost(0), vio_wt(w) { }
    ~VRPStateManager() { }
    void UpdateTimeTable(RoutePlan&);
    void RandomState(RoutePlan&);
    int SampleState(RoutePlan&, unsigned);
    bool CheckConsistency(const RoutePlan&) const { return true; }
    int CostFunction(const RoutePlan&) const;
    int Objective(const RoutePlan&) const;
    int Violations(const RoutePlan&) const;
    // int operator() (unsigned r, unsigned o) const { return timetable[r][o]; }
    // int& operator() (unsigned r, unsigned o ) { return timetable[r][o]; }
    // const std::vector<int>& operator[](unsigned i) const { return timetable[i]; }
    // std::vector<int>& operator[](unsigned i) { return timetable[i]; }
 private:
    void ResetState(RoutePlan&);
    int ComputeDateViolationCost(const RoutePlan&, int) const;    // s1
    int ComputeTimeViolationCost(const RoutePlan&, int) const;    // s2
    int ComputeOptOrderCost(const RoutePlan&, int) const;         // s3
    int ComputeTranportationCost(const RoutePlan&) const;         // s4
    int ComputeCapExceededCost(const RoutePlan&, int) const;      // h1
    int ComputeLateReturnCost(const RoutePlan&, int) const;       // h2
    mutable int num_order_late_return;
    mutable int cap_vio_cost;
    int vio_wt;
};

#endif
