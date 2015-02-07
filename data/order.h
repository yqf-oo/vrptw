#ifndef _ORDER_H
#define _ORDER_H
#include <iostream>
#include <string>

class Order
{
	friend std::istream& operator>>(const std::istream&, const Order&);
public:
	Order() { }
	Order(std::string id_o, std::string id_cli, unsigned qty, bool m, 
		  int rd = 0, int dd = 0):
		id(id_o), id_client(id_cli), quantity(q), mandatory(m), 
		date_window(rd, dd) { }
	unsigned get_qty() const { return quantity; }
	bool is_mandatory() const { return mandatory; }
private:
	std::string id, id_client;
	unsigned quantity;
	bool mandatory;
	pair<int, int> date_window;
};
#endif