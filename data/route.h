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
    unsigned size() const { return orders.size(); }
    int length(const ProbInput&) const;
    int demand(const ProbInput&) const;
    int get_day() const { return day; }
    int get_vehicle() const { return vehicle; }
    bool IsExcList() const { return exc_list; }
    void push_back(int order_index) { orders.push_back(order_index); }
    void erase(unsigned pos) { orders.erase(orders.begin() + pos); }
    void insert(unsigned pos, int order) {
        orders.insert(orders.begin() + pos, order);
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
    friend std::ostream& operator<<(std::ostream&, const RoutePlan&);

 public:
    RoutePlan(const ProbInput& pi): in(pi) { Allocate(); }
    RoutePlan(const RoutePlan &rp):
        in(rp.in), routes(rp.routes), plan(rp.plan), timetable(rp.timetable) { }
    void AddOrder(int , unsigned, unsigned, bool);
    void AddRoute(const Route &r) { routes.push_back(r); }
    void ResizeTimetable(unsigned sz) { timetable.resize(sz); }
    void ResizeRouteTimetable(unsigned i, unsigned sz, int val) {
        if (i >= timetable.size())
            return;
        timetable[i].resize(sz, val);
    }
    unsigned size() const { return routes.size(); }
    // int& get_est(int route, int order) { return timetable[route][order]; }
    const Route& operator[] (int i) const { return routes[i]; }
    Route& operator[] (int i) { return routes[i]; }
    int vd_route(unsigned v, unsigned d) const { return plan[v][d]; }
    int& vd_route(unsigned v, unsigned d) { return plan[v][d]; }
    std::vector<int>& routetime(int i) { return timetable[i]; }
    int operator() (unsigned r, unsigned o) const { return timetable[r][o]; }
    int& operator() (unsigned r, unsigned o) { return timetable[r][o]; }
    RoutePlan& operator=(const RoutePlan &rp) {
        routes = rp.routes;
        plan = rp.plan;
        timetable = rp.timetable;
        return *this;
    }
    // check whether a route plan is feasible
    bool CheckFeasibility();

 private:
    void Allocate();
    const ProbInput &in;
    std::vector<Route> routes;
    std::vector<std::vector<int> > plan;
    std::vector<std::vector<int> > timetable;
};
#endif
