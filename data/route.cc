#include "data/route.h"
#include <vector>
#include <string>
#include <utility>

std::istream& operator>>(std::istream &is, RoutePlan &rp) {
    return is;
}

std::ostream& operator<<(std::ostream &os, const RoutePlan &rp) {
    int day = -1;
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        if (rp[i].get_day() != day) {
            day = rp[i].get_day();
            os << "Day " << day + 1 <<" :" << std::endl;
        }
        const Vehicle& v = rp.in.VehicleVect(rp[i].get_vehicle());
        os << "# " << i << v.get_id() << "(" << v.get_cap() << ")"
           << rp[i].get_num_order() << ":";

        for (unsigned j = 0; j < rp[i].size(); ++j) {
            const OrderGroup &og = rp.in.OrderGroupVect(j);
            for (unsigned k = 0; k < og.size(); ++k)
                os << " " << og[k];
        }
        os << " [" << rp[i].demand() << "]" << std::endl;
    }

    // unscheduled
    os << std::endl;
    unsigned uns = rp.size() - 1;
    os << "Unscheduled " << rp[uns].get_num_order() << ":";
    for (unsigned i = 0; i < rp[uns].size(); ++i) {
        const OrderGroup &og = rp.in.OrderGroupVect(rp[uns][i]);
        for (unsigned k = 0; k < og.size(); ++k)
            os << " " << og[k];
    }
    os << " [" << rp[uns].demand() << "]" << std::endl;
    return os;
}

void RoutePlan::AddOrder(int order_index, unsigned day,
                         unsigned vid, bool unscheduled) {
    int route_index = routes_.size() - 1;
    if (!unscheduled)
        route_index = day * in.get_num_vehicle() + vid;
    routes_[route_index].push_back(order_index);
}

void RoutePlan::Allocate() {
    int plan_size = in.get_dayspan() * in.get_num_vehicle() + 1;
    for (int i = 0; i < plan_size; ++i)
        routes_.push_back(Route(i, false, in));
    routes_[plan_size - 1].set_ext_list(true);
    timetable_.resize(routes_.size() - 1);
    // plan.resize(num_vehicle, std::vector<int>(day_span, -1));
}

unsigned Route::get_num_order() const {
    unsigned sz = 0;
    for (unsigned i = 0; i < orders.size(); ++i) {
        const OrderGroup &og = in.OrderGroupVect(orders[i]);
        sz += og.size();
    }
    return sz;
}

int Route::length() const {
    int len = 0;
    std::string client_from(in.get_depot());
    for (unsigned i = 0; i < orders.size(); ++i) {
        const OrderGroup &og = in.OrderGroupVect(orders[i]);
        std::string client_to = og.get_client();
        len += in.get_distance(client_from, client_to);
        client_from = client_to;
    }
    len += in.get_distance(client_from, in.get_depot());
    return len;
}

int Route::demand() const {
    int demand = 0;
    for (unsigned i = 0; i < orders.size(); ++i) {
        demand += in.OrderGroupVect(orders[i]).get_demand();
    }
    return demand;
}
