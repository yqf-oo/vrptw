#ifndef _VRP_TABU_LIST_MANAGER_H_
#define _VRP_TABU_LIST_MANAGER_H_

#include <helpers/TabuListManager.hh>
#include "data/route.h"
#include "data/neighbor.h"

class InsMoveTabuListManager: public TabuListManager<RoutePlan, InsMove> {
 public:
    InsMoveTabuListManager(unsigned i):
        TabuListManager<RoutePlan, InsMove>(), index(i) { }
    bool Inverse(const InsMove&, const InsMove&) const;
 protected:
    unsigned index;
    // bool ListMember(const InsMove&) const;
};

class InterSwapTabuListManager: public TabuListManager<RoutePlan, InterSwap> {
 public:
    InterSwapTabuListManager():
        TabuListManager<RoutePlan, InterSwap>() { }
    bool Inverse(const InterSwap&, const InterSwap&) const;
 protected:
    // bool ListMember(const InsMove&) const;
};

class IntraSwapTabuListManager: public TabuListManager<RoutePlan, IntraSwap> {
 public:
    IntraSwapTabuListManager():
        TabuListManager<RoutePlan, IntraSwap>() { }
    bool Inverse(const IntraSwap&, const IntraSwap&) const;
 protected:
    // bool ListMember(const InsMove&) const;
};

#endif
