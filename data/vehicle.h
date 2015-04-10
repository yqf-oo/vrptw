#ifndef _VEHICLE_H_
#define _VEHICLE_H_
#include <iostream>
#include <string>

class Vehicle {
    friend std::istream& operator>>(std::istream&, Vehicle&);
 public:
    // unsigned get_capacity() const { return capacity; }
    // unsigned get_fixed_cost() const { return fixed_cost; }
    // std::string get_id() const {return id; }
    // std::string get_carrier() const { return id_carrier; }
    std::string get_id() const { return id_; }
    unsigned get_cap() const { return capacity_; }
    std::string get_carrier() const { return id_carrier_; }
    unsigned fixed_cost() const { return fixed_cost_; }
 private:
    std::string id_, id_carrier_;
    unsigned capacity_, fixed_cost_;
};
#endif
