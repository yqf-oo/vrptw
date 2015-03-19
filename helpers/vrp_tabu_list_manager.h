#ifndef _VRP_TABU_LIST_MANAGER_H_
#define _VRP_TABU_LIST_MANAGER_H_

#include <helpers/TabuListManager.hh>
#include "data/route.h"
#include "data/neighbor.h"

class InsMoveTabuListManager: public TabuListManager<RoutePlan, InsMove> {
 public:
    InsMoveTabuListManager(unsigned min_tenture, unsigned max_tentrue):
        TabuListManager<RoutePlan, InsMove>(min_tenture, max_tentrue) { }
    bool Inverse(const InsMove&, const InsMove&) const;
 protected:
    // bool ListMember(const InsMove&) const;
};

class InterSwapTabuListManager: public TabuListManager<RoutePlan, InterSwap> {
 public:
    InterSwapTabuListManager(unsigned min_tenture, unsigned max_tentrue):
        TabuListManager<RoutePlan, InterSwap>(min_tenture, max_tentrue) { }
    bool Inverse(const InterSwap&, const InterSwap&) const;
 protected:
    // bool ListMember(const InsMove&) const;
};

class IntraSwapTabuListManager: public TabuListManager<RoutePlan, IntraSwap> {
 public:
    IntraSwapTabuListManager(unsigned min_tenture, unsigned max_tentrue):
        TabuListManager<RoutePlan, IntraSwap>(min_tenture, max_tentrue) { }
    bool Inverse(const IntraSwap&, const IntraSwap&) const;
 protected:
    // bool ListMember(const InsMove&) const;
};

#endif
