#include <iostream>
#include <utility>
#include "data/order.h"

std::istream& operator>>(std::istream &is, Order &o) {
    is >> o.id >> o.id_client >> o.quantity >> o.mandatory
       >> o.date_window.first >> o.date_window.second;
    return is;
}

std::ostream& operator<<(std::ostream &os, const OrderGroup &og) {
    os << "Mand:" << og.mandatory << " members: ";
    for (unsigned i = 0; i < og.size(); ++i) {
        os << " " << og.members[i];
    }
    return os;
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

bool OrderGroup::IsGroupCompatible(const Order &o) const {
    return ((o.get_client() == id_client)
            && (o.IsMandatory() == mandatory)
            && (o.get_dw() == date_window));
}
