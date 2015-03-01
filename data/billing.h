#ifndef _BILLING_H_
#define _BILLING_H_
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

class BillingCostComponent;

class Billing {
 public:
    Billing(std::string i, std::string t):
        id(i), type(t) { }
    virtual ~Billing() { }
    void SetCostComponent(BillingCostComponent* cc) {
        cost_component = cc;
    }
    BillingCostComponent& GetCostComponent() const {
        return (*cost_component);
    }
    // size_t CostComponents() const { return cost_component.size(); }
    std::string get_id() const { return id; }
    std::string get_type() const { return type; }
 protected:
    std::string id, type;
    BillingCostComponent *cost_component;
};

// KM BILLING (cost function 1)
class KmBilling: public Billing {
 public:
    KmBilling(std::string i, std::string t, unsigned rate):
        Billing(i, t), km_rate(rate) { }
    ~KmBilling() { delete cost_component; }
    unsigned get_km_rate() const { return km_rate; }
 private:
    unsigned km_rate;
};

// LOAD KM BILLING (cost function 2)
class LoadKmBilling: public Billing {
 public:
    LoadKmBilling(std::string i, std::string t):
        Billing(i, t) { }
    ~LoadKmBilling() { delete cost_component; }
    unsigned get_km_rate() const { return km_rate; }
    unsigned get_full_load() const { return full_load_value; }
    unsigned get_load_cost(unsigned r_index) const {
        assert(r_index < load_cost.size());
        return load_cost[r_index];
    }
    void ReadInputData(istream&, unsigned);
 private:
    unsigned km_rate;
    unsigned full_load_value;
    std::vector<unsigned> load_cost;
};

// VAR LOAD BILLING (cost function 3)
class VarLoadBilling: public Billing {
 public:
    VarLoadBilling(std::string i, std::string t):
        Billing(i, t) { }
    ~VarLoadBilling() { delete cost_component; }
    unsigned get_load_cost(unsigned r_index, unsigned range) {
        assert(range < num_range && r_index < load_cost.size());
        return load_cost[r_index][range];
    }
    unsigned operator[] (unsigned i) const { return levels[i]; }
    unsigned& operator[] (unsigned i ) { return levels[i]; }
    void ReadInputData(istream&, unsigned);
 private:
    std::vector<std::vector<unsigned>> load_cost;
    std::vector<unsigned> levels;
};

// LOAD BILLING (cost funtion 4)
class LoadBilling: public Billing {
 public:
    LoadBilling(std::string i, std::string t):
        Billing(i, t) { }
    ~LoadBilling() { delete cost_component; }
    void ReadInputData(istream&, unsigned);
    unsigned get_load_cost(unsigned r_index) const {
        assert(r_index < load_cost.size());
        return load_cost[r_index];
    }
 private:
    std::vector<unsigned> load_cost;
};

#endif
