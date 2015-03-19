#ifndef _BILLING_H_
#define _BILLING_H_
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "helpers/billing_cost_component.h"

// class BillingCostComponent;

class Billing {
 public:
    Billing(std::string i, std::string t):
        id(i), type(t) { }
    virtual ~Billing() { delete cost_component; }
    void SetCostComponent(BillingCostComponent* cc) {
        cost_component = cc;
    }
    const BillingCostComponent& GetCostComponent() const {
        return (*cost_component);
    }
    // size_t CostComponents() const { return cost_component.size(); }
    std::string get_id() const { return id; }
    std::string get_type() const { return type; }

    virtual int get_km_rate() const { return 0; }

 protected:
    std::string id, type;
    BillingCostComponent *cost_component;
};

// KM BILLING (cost function 1)
class KmBilling: public Billing {
 public:
    KmBilling(std::string i, std::string t, unsigned rate):
        Billing(i, t), km_rate(rate) { }
    ~KmBilling() { }
    int get_km_rate() const { return km_rate; }
 private:
    int km_rate;
};

// LOAD KM BILLING (cost function 2)
class LoadKmBilling: public Billing {
 public:
    LoadKmBilling(std::string i, std::string t):
        Billing(i, t) { }
    ~LoadKmBilling() { }
    int get_km_rate() const { return km_rate; }
    int get_full_load() const { return full_load_value; }
    int get_load_cost(unsigned r_index) const {
        assert(r_index < load_cost.size());
        return load_cost[r_index];
    }
    void ReadInputData(std::istream&, int);
 private:
    int km_rate;
    int full_load_value;
    std::vector<int> load_cost;
};

// VAR LOAD BILLING (cost function 3)
class VarLoadBilling: public Billing {
 public:
    VarLoadBilling(std::string i, std::string t):
        Billing(i, t) { }
    ~VarLoadBilling() { }
    int get_num_range() const { return num_range; }
    int get_load_cost(unsigned r_index, unsigned range) const {
        assert(r_index < load_cost.size());
        return load_cost[r_index][range];
    }
    int get_level(unsigned i) const { return levels[i]; }
    void ReadInputData(std::istream&, int);
 private:
    int num_range;
    std::vector<std::vector<int> > load_cost;
    std::vector<int> levels;
};

// LOAD BILLING (cost funtion 4)
class LoadBilling: public Billing {
 public:
    LoadBilling(std::string i, std::string t):
        Billing(i, t) { }
    ~LoadBilling() { }
    void ReadInputData(std::istream&, int);
    int get_load_cost(unsigned r_index) const {
        assert(r_index < load_cost.size());
        return load_cost[r_index];
    }
 private:
    std::vector<int> load_cost;
};

#endif
