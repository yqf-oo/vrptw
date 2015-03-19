#include <iostream>
#include "data/vehicle.h"

std::istream& operator>>(std::istream &is, Vehicle &v) {
    is >> v.id >> v.capacity >> v.fixed_cost >> v.id_carrier;
    return is;
}
