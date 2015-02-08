#ifndef _ORDER_H
#define _ORDER_H
#include <iostream>
#include <string>
#include <vector>
#include <utility>

class Order {
    friend std::istream& operator>>(std::istream&, const Order&);
 public:
    Order() { }
    Order(const Order &o):
        id(o.id), id_client(o.id_client), quantity(o.quantity),
        mandatory(o.mandatory), date_window(o.date_window)
        { }
    Order(std::string id_o, std::string id_cli, unsigned qty, bool m,
          int rd = 1, int dd = 1):
        id(id_o), id_client(id_cli), quantity(q), mandatory(m),
        date_window(rd, dd) { }
 protected:
    std::string id, id_client;
    unsigned quantity;
    bool mandatory;
    std::pair<int, int> date_window;
};

class OrderGroup : public Order{
 public:
    OrderGroup() { }
    OrderGroup(const Order &o): Order(o), members(1, o.id) { }
    OrderGroup(const OrderGroup &og): Order(og), members(og.members) { }
    unsigned size() const { return members.size(); }
    void insert(const Order &);
    bool GroupCompatibility(const Order& o) const {
        return ((o.id == id) && (o.quantity == quantity)
                 && (o.date_window == date_window));
    }
    string& operator[](unsigned i) { return members[i]; }
    OrderGroup& operator=(const OrderGroup&);
 private:
    std::vector<string> members;
};
#endif
