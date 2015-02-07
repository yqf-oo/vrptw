#include <iostream>
#include <utility> 

std::istream& operator>>(const std::istream &is, const Client &c){
	is >> c.id >> c.id_region >> c.time_window.first
	   >> c.time_window.second >> c.serv_duration;
	return is;
}