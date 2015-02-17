#ifndef _BILLING_COST_COMPONENT_H_
#define _BILLING_COST_COMPONENT_H_
#include <iostream>
#include <string>
#include <utility>
#include "data/prob_input.h"
#include "data/route.h"

class BillingCostComponent {
 public:
    BillingCostComponent(const ProbInput &i, int w, string n):
        in(i), weight(w), name(n) { }
    virtual int ComputeCost(const Route &r) const = 0;
    int Cost(const Route &r) const {
        return weight * ComputeCost(r);
    }
    string Name() const { return name; }
    virtual void PrintViolations(const Route &r, unsigned route_index,
                                 std::ostream &os = std::cout);
    void SetWeight(const unsigned w) { weight = w; }
    int Weight() const { return weight; }
    virtual ~BillingCostComponent() {}
 protected:
    std::pair<unsigned, unsigned> MaxRateLoad(const Route&);
    const ProbInput &in;
    const string name;
    int weight;
};

class DistanceBillingCostComponent: public BillingCostComponent {
 public:
    DistanceBillingCostComponent(const ProbInput &in,  int weight):
        BillingCostComponent(in, weight, "DistanceBillingCostComponent") {}
    int ComputeCost(const Route &r) const;
};


class LoadFarestClientCostComponent: public BillingCostComponent {
 public:
    LoadFarestClientCostComponent(const ProbInput &in, int weight):
        BillingCostComponent(in, weight, "LoadFarestClientCostComponent")
    // pair<unsigned, unsigned> MaxRateLoad(const Route &r) const;
    int  ComputeCost(const Route &r) const;
};

// LoadRangeBillingCostComponent
// This class compute the cost dependent on the load and on the distance.
class LoadRangeBillingCostComponent: public BillingCostComponent {
 public:
    LoadRangeBillingCostComponent(const ProbInput &in, int weight):
        BillingCostComponent(in, weight, "LoadRangeBillingCostComponent") {}
    int  ComputeCost(const Route &r) const;
 private:
    unsigned FindRange(const Route &r, unsigned load) const;
    unsigned MaxRate(const Route &r, unsigned range) const;
};

// DistanceLoadBillingCostComponent
// This class compute the cost dependent on the load and on the distance.
class DistanceLoadBillingCostComponent: public BillingCostComponent {
 public:
    DistanceLoadBillingCostComponent(const ProbInput &in,  int weight):
        BillingCostComponent(in, weight, "DistanceLoadBillingCostComponent") {}
    // pair<unsigned, unsigned> MaxRateLoad(const Route &r) const;
    int ComputeCost(const Route &r) const;
 private:
    bool IsFull(const Route &r) const;
};

// LoadClientDependetBillingCostComponent
// This class compute the cost dependent on the load
// and on the province for each client(load*cost_coef).
class LoadClientDependentCostComponent: public BillingCostComponent {
 public:
  LoadClientDependentCostComponent(const ProbInput &in, int weight):
    BillingCostComponent(in, weight, "LoadClientDependentCostComponent") {}
  int  ComputeCost(const Route &r) const;
};

#endif
