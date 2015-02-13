#ifndef _ROUTE_H
#define _ROUTE_H
#include <iostream>
#include <list>
#include "data/prob_input.h"

class Route {
 public:
    Route(int d, int v_ind, bool ex, const std::vector<int> o):
        day(d), vehicle_index(v_ind), exc_list(ex), orders(o) { }
    unsigned get_day() const { return day; }
    bool IsExcList() const { return exc_list; }
    void AddOrder(int order_index) { orders.push_back(order_index); }
 private:
    int day, vehicle_index;
    bool exc_list;
    std::vector<int> orders;
};

class RoutePlan {
 public:
    RoutePlan(const ProbInput& pi): in(pi) { }
    void AddRoute(unsigned d, const Route &r) {
        assert(d < plan.size());
        plan[d].push_back(r);
    }
    // check whether a route plan is feasible
    bool CheckFeasibility();
 private:
    ProbInput &in;
    std::vector<std::vector<Route>> plan;
};
#endif
