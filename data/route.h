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
    Route(const Route &r): day(r.day), vehicle(r.vehicle),
                           exc_list(r.exc_list), orders(r.orders) { }
    int size() const { return orders.size(); }
    unsigned length(const ProbInput&) const;
    unsigned demand(const ProbInput&) const;
    unsigned get_day() const { return day; }
    unsigned get_vehicle() const { return vehicle; }
    bool IsExcList() const { return exc_list; }
    void push_back(int order_index) { orders.push_back(order_index); }
    void erase(unsigned pos) { orders.erase(orders.cbegin() + pos); }
    void insert(unsigned pos, int order) {
        orders.insert(orders.cbegin() + pos, order);
    }
    const int& operator[] (int i) const { return orders[i]; }
    int& operator[] (int i) { return orders[i]; }
    Route& operator=(const Route &r) {
        day = r.day;
        vehicle = r.vehicle;
        orders = r.orders;
        return *this;
    }
 private:
    int day, vehicle;
    bool exc_list;
    std::vector<int> orders;
};

class RoutePlan {
 public:
    RoutePlan(const ProbInput& pi): in(pi) { Allocate(); }
    RoutePlan(const RoutePlan &rp): 
        in(rp.pi), routes(rp.routes), plan(rp.plan) { }
    void AddOrder(int , unsigned, unsigned, bool);
    void AddRoute(const Route &r) { routes.push_back(r); }
    int size() const { return routes.size(); }
    // int get_est(int route, int order) const { return timetable[route][order]; }
    // int& get_est(int route, int order) { return timetable[route][order]; }
    const Route& operator[] (int i) const { return routes[i]; }
    Route& operator[] (int i) const { return routes[i]; }
    const int& operator() (int veh, int day) const { return plan[veh][day]; }
    int& operator() (int veh, int day) { return plan[veh][day]; }
    RoutePlan& operator=(const RoutePlan &rp) {
        routes = rp.routes;
        plan = rp.plan;
        return *this;
    }
    // check whether a route plan is feasible
    bool CheckFeasibility();
 private:
    void Allocate();
    ProbInput &in;
    std::vector<Route> routes;
    std::vector<std::vector<int>> plan;
};
#endif
