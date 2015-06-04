#ifndef _ROUTE_H_
#define _ROUTE_H_
#include <iostream>
#include <vector>
#include "data/prob_input.h"

class Route {
 public:
    Route(int i, bool e, const ProbInput &p):
        id(i), exc_list(e), orders(0), in(p) { }
    // Route(std::vector<int> ov):
    //     day(0), vehicle(0), exc_list(true), orders(ov) { }
    Route(const Route &r):
        id(r.id), exc_list(r.exc_list), orders(r.orders), in(r.in) { }
    unsigned size() const { return orders.size(); }
    int length() const;
    int demand() const;
    int get_day() const { return id / in.get_num_vehicle(); }
    int get_vehicle() const { return id % in.get_num_vehicle(); }
    bool IsExcList() const { return exc_list; }
    void set_ext_list(bool unschduled) { exc_list = unschduled; }
    unsigned get_num_order() const;
    void push_back(int order_index) { orders.push_back(order_index); }
    void erase(unsigned pos) { orders.erase(orders.begin() + pos); }
    void insert(unsigned pos, int order) {
        orders.insert(orders.begin() + pos, order);
    }
    void clear() { orders.clear(); }
    const int& operator[] (int i) const { return orders[i]; }
    int& operator[] (int i) { return orders[i]; }
    Route& operator=(const Route &r) {
        id = r.id;
        orders = r.orders;
        return *this;
    }

 private:
    int id;
    bool exc_list;
    std::vector<int> orders;
    const ProbInput &in;
};

class RoutePlan {
    friend std::istream& operator>>(std::istream&, RoutePlan&);
    friend std::ostream& operator<<(std::ostream&, const RoutePlan&);

 public:
    RoutePlan(const ProbInput& pi): in(pi), vios(0) { Allocate(); }
    RoutePlan(const RoutePlan &rp):
        in(rp.in), routes_(rp.routes_), timetable_(rp.timetable_), vios(rp.vios) { }
    void AddOrder(int , unsigned, unsigned, bool);
    void AddRoute(const Route &r) { routes_.push_back(r); }
    // void ResizeTimetable(unsigned sz) { timetable.resize(sz); }
    void ResizeRouteTimetable(unsigned i, unsigned sz, int val) {
        if (i >= timetable_.size())
            return;
        timetable_[i].resize(sz, val);
    }
    unsigned size() const { return routes_.size(); }
    unsigned num_routes() const { return routes_.size() - 1; }
    const Route& operator[] (int i) const { return routes_[i]; }
    Route& operator[] (int i) { return routes_[i]; }
    // int vd_route(unsigned v, unsigned d) const { return plan[v][d]; }
    // int& vd_route(unsigned v, unsigned d) { return plan[v][d]; }
    std::vector<int>& timetable(int i) { return timetable_[i]; }
    const std::vector<int>& timetable(int i) const { return timetable_[i]; }
    int operator() (unsigned r, unsigned o) const { return timetable_[r][o]; }
    int& operator() (unsigned r, unsigned o) { return timetable_[r][o]; }
    RoutePlan& operator=(const RoutePlan &rp) {
        routes_ = rp.routes_;
        timetable_ = rp.timetable_;
        vios = rp.vios;
        return *this;
    }
    // check whether a route plan is feasible
    bool CheckFeasibility();
    void set_vio(int v) const { vios = v; }
    int get_vio() const { return vios; }

 private:
    void Allocate();
    const ProbInput &in;
    std::vector<Route> routes_;
    // std::vector<std::vector<int> > plan;
    std::vector<std::vector<int> > timetable_;
    mutable int vios;
};
#endif
