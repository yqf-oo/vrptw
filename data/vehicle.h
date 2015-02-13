#ifndef _VEHICLE_H
#define _VEHICLE_H
#include <iostream>
#include <string>

class Vehicle {
    friend std::istream& operator>>(std::istream&, const Vehicle&);
 public:
    // unsigned get_capacity() const { return capacity; }
    // unsigned get_fixed_cost() const { return fixed_cost; }
    // std::string get_id() const {return id; }
    // std::string get_carrier() const { return id_carrier; }
    std::string get_id() const { return id; }
    unsigned get_cap const { return capacity; }
    std::string get_carrier() const { return id_carrier; }
 private:
    std::string id, id_carrier;
    unsigned capacity, fixed_cost;
};
#endif
