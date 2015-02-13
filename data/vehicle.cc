#include <iostream>
#include "data/vehicle.h"

std::istream& Vehicle::operator>>(std::istream &is, const Vehicle &v) {
    is >> v.id >> v.capacity >> v.fixed_cost >> v.id_carrier;
    return is;
}
