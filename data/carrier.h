#ifndef _CARRIER_H
#define _CARRIER_H
#include <iostream>
#include <vector>
#include <string>
#include <utility>

class Order
{
	friend std::istream& operator>>(const std::istream&, const Carrier&);
public:
	std::string get_billing() const { return id_billing; }
private:
	string id, id_billing;
	std::vector<std::string> incompat_regions;
};
#endif