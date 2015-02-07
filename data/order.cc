#include <iostream>
#include <utility> 

std::istream& operator>>(const std::istream &is, const Order &o){
	is >> o.id >> o.id_client >> o.quantity >> o.mandatory
	   >> o.date_window.first >> o.date_window.second;
	return is;
}