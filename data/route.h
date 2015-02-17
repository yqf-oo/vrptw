#ifndef _ROUTE_H
#define _ROUTE_H
#include <iostream>
#include <vector>
#include "data/prob_input.h"

class Route {
 public:
    Route(int oid, int d, int vi, bool ex):
        day(d), vehicle(vi), exc_list(ex), orders(1, oid) { }
    Route(std::vector<int> ov):
        day(0), vehicle(0), exc_list(true), orders(ov) { }
    int size() const { return orders.size(); }
    int length(cost ProbInput&) const;
    unsigned get_day() const { return day; }
    unsigned get_vehicle() const { return vehicle; }
    bool IsExcList() const { return exc_list; }
    void AddOrder(int order_index) { orders.push_back(order_index); }
    const int& operator[] (int i) const { return orders[i]; }
    int& operator[] (int i) { return orders[i]; }
 private:
    int day, vehicle;
    bool exc_list;
    std::vector<int> orders;
};

class RoutePlan {
 public:
    RoutePlan(const ProbInput& pi): in(pi) { Allocate(); }
    void AddOrder(int , unsigned, unsigned, bool);
    void AddRoute(const Route &r) { routes.push_back(r); }
    int size() const { return routes.size(); }
    int get_est(int route, int order) const { return timetable[route][order]; }
    const Route& operator[] (int i) const { return routes[i]; }
    Route& operator[] (int i) const { return routes[i]; }
    const int& operator() (int veh, int day) const { return plan[veh][day]; }
    int& operator() (int veh, int day) { return plan[veh][day]; }
    // check whether a route plan is feasible
    bool CheckFeasibility();
 private:
    void Allocate();
    ProbInput &in;
    std::vector<Route> routes;
    std::vector<std::vector<int>> timetable;
    std::vector<std::vector<int>> plan;
};
#endif
