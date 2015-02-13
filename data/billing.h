#ifndef _BILLING_H_
#define _BILLING_H_
#include <iostream>
#include <string>
#include <vector>

class Billing {
 public:
    Billing(std::string i, std::string t):
        id(i), type(t) { }
    virtual ~Billing() { }
    std::string get_id() const { return id; }
    std::string get_type() const { return type; }
 protected:
    std::string id, type;
};

// KM BILLING (cost function 1)
class KmBilling: public Billing {
 public:
    KmBilling(std::string i, std::string t, unsigned rate):
        Billing(i, t), km_rate(rate) { }
    unsigned get_km_rate() const { return km_rate; }
 private:
    unsigned km_rate;
};

// LOAD KM BILLING (cost function 2)
class LoadKmBilling: public Billing {
 public:
    LoadKmBilling(std::string i, std::string t):
        Billing(i, t) { }
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
    unsigned get_range() const { return num_range; }
    unsigned get_level(unsigned i) const {
        assert(i < num_range);
        return levels[i];
    }
    unsigned get_load_cost(unsigned r_index, unsigned range) {
        assert(range < num_range && r_index < load_cost.size());
        return load_cost[r_index][range];
    }
    void ReadInputData(istream&, unsigned);
 private:
    unsigned num_range;
    std::vector<std::vector<unsigned>> load_cost;
    std::vector<unsigned> levels;
};

// LOAD BILLING (cost funtion 4)
class LoadBilling: public Billing {
 public:
    LoadBilling(std::string i, std::string t):
        Billing(i, t) { }
    void ReadInputData(istream&, unsigned);
    unsigned get_load_cost(unsigned r_index) const {
        assert(r_index < load_cost.size());
        return load_cost[r_index];
    }
 private:
    std::vector<unsigned> load_cost;
};

#endif
