#include "data/route.h"
#include <vector>
#include <string>
#include <utility>

std::istream& operator>>(std::istream &is, RoutePlan &rp) {
    return is;
}

std::ostream& operator<<(std::ostream &os, const RoutePlan &rp) {
    int day = -1;
    for (unsigned i = 0; i < rp.size(); ++i) {
        if (rp[i].get_day() != day) {
            day = rp[i].get_day();
            os << "Day " << day <<" :" << std::endl;
        }
        const Vehicle& v = in.VehicleVect(rp[i].get_vehicle());
        os << "# " << i << v.get_id() << "(" << v.get_cap() << ")"
           << rp[i].get_num_order(in) << ":";

        for (unsigned j = 0; j < rp[i].size(); ++j) {
            const OrderGroup &og = in.OrderGroupVect(j);
            for (unsigned k = 0; k < og.size(); ++k)
                os << " " << og[k].get_id();
        }
        os << " [" << rp[i].demand(rp.in) << "]" << std::endl;
    }

    // unscheduled
    os << std::endl;
    unsigned uns = rp.size() - 1;
    os << "Unscheduled " << rp[uns].get_num_order(in) << ":";
    for (unsigned i = 0; i < rp[uns].size(); ++i) {
        const OrderGroup &og = in.OrderGroupVect(j);
        for (unsigned k = 0; k < og.size(); ++k)
            os << " " << og[k].get_id();
    }
    os << " [" << rp[uns].demand(in) << "]" << std::endl;
    return os;
}

void RoutePlan::AddOrder(int order_index, unsigned day,
                         unsigned vid, bool unscheduled) {
    int route_index = routes.size() - 1;
    if (!unscheduled)
        route_index = day * in.get_num_vehicle() + vid;
    routes[route_index].push_back(order_index);
}

void RoutePlan::Allocate() {
    int day_span = in.get_dayspan();
    int num_vehicle = in.get_num_vehicle();
    routes.resize(day_span * num_vehicle + 1, Route());
    timetable.resize(routes.size());
    // plan.resize(num_vehicle, std::vector<int>(day_span, -1));
}

unsigned Route::size(const ProbInput &in) const {
    unsigned sz = 0;
    for (unsigned i = 0; i < orders.size(); ++i) {
        const OrderGroup &og = in.OrderGroupVect(orders[i]);
        sz += og.size();
    }
    return sz;
}

int Route::length(const ProbInput& in) const {
    int len = 0;
    std::string client_from(in.get_depot());
    for (unsigned i = 0; i < orders.size(); ++i) {
        const OrderGroup &og = in.OrderGroupVect(orders[i]);
        // std::string client_to = in.OrderVect(orders[i]).get_client();
        std::string client_to = og.get_client();
        len += in.get_distance(client_from, client_to);
        client_from = client_to;
    }
    len += in.get_distance(client_from, in.get_depot());
    return len;
}

int Route::demand(const ProbInput& in) const {
    int demand = 0;
    for (unsigned i = 0; i < orders.size(); ++i) {
        demand += in.OrderGroupVect(orders[i]).get_demand();
    }
    return demand;
}
