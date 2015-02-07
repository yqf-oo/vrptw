#include <vehicle.h>
#include <iostream>

std::istream& operator>>(const std::istream &is, const vehicle &o){
	is >> v.id >> v.capacity >> v.fixed_cost >> v.id_carrier;
	return is;
}