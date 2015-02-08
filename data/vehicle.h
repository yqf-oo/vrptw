#ifndef _VEHICLE_H
#define _VEHICLE_H
#include <iostream>
#include <string>

class Vehicle {
    friend std::istream& operator>>(std::istream&, const Vehicle&);
 public:
    Vehicle() { }
    Vehicle(std::string id_v, unsigned cap, unsigned cost, std::string id_cr):
        id(id_v), id_carrier(id_cr), capacity(cap), fixed_cost(cost) { }
    // unsigned get_capacity() const { return capacity; }
    // unsigned get_fixed_cost() const { return fixed_cost; }
    // std::string get_id() const {return id; }
    // std::string get_carrier() const { return id_carrier; }
 private:
    std::string id, id_carrier;
    unsigned capacity, fixed_cost;
};
#endif
