#include <iostream>
#include <utility>
#include "data/order.h"

std::istream& operator>>(std::istream &is, Order &o) {
    is >> o.id >> o.id_client >> o.quantity >> o.mandatory
       >> o.date_window.first >> o.date_window.second;
    return is;
}

OrderGroup& OrderGroup::operator=(const OrderGroup &og) {
    id = og.id;
    id_client = og.id_client;
    quantity = og.quantity;
    date_window = og.date_window;
    members = og.members;
    return *this;
}

void OrderGroup::insert(const Order &o) {
    members.push_back(o.get_id());
    quantity += o.get_demand();
}
