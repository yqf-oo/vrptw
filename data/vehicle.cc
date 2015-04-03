#include <iostream>
#include "data/vehicle.h"

std::istream& operator>>(std::istream &is, Vehicle &v) {
    is >> v.id_ >> v.capacity_ >> v.fixed_cost_ >> v.id_carrier_;
    v.fixed_cost_ *= 1000;
    return is;
}
