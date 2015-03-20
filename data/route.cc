#include "data/route.h"
#include <vector>
#include <string>
#include <utility>

std::istream& operator>>(std::istream &is, RoutePlan &rp) {
    return is;
}

std::ostream& operator<<(std::ostream &os, const RoutePlan &rp) {
    for (int i = 1; i <= rp.in.get_dayspan(); ++i) {
        os << "Day " << i <<" :" << std::endl;
        for (int j = 0; j < rp.in.get_num_vehicle(); ++j) {
            int rid = rp.plan[j][i - 1];
            if (rid != -1) {
                std::string vehicle = rp.in.VehicleVect(j).get_id();
                unsigned veh_cap = rp.in.VehicleVect(j).get_cap();
                os << "# " << rid << vehicle << "(" << veh_cap << ")"
                   << rp[rid].size() << ":";
                for (unsigned k = 0; k < rp[rid].size(); ++k)
                    os << " " << rp.in.OrderVect(rp[rid][k]).get_id();
                os << " [" << rp[rid].demand(rp.in) << "]" << std::endl;
            }
        }
    }
    os << std::endl;
    assert(rp.size() > 1);
    unsigned unscheduled = rp.size() - 1;
    os << "Unscheduled " << rp[unscheduled].size() << ":";
    for (unsigned i = 0; i < rp[unscheduled].size(); ++i)
        os << " " << rp.in.OrderVect(rp[unscheduled][i]).get_id();
    os << " [" << rp[unscheduled].demand(rp.in) << "]" << std::endl;
    return os;
}

void RoutePlan::AddOrder(int order_index, unsigned day,
                         unsigned vid, bool unscheduled) {
    int &rid = plan[vid][day];
    if (rid != -1) {
        routes[rid].push_back(order_index);
    } else {
        routes.push_back(Route(order_index, day, vid, unscheduled));
        rid = routes.size() - 1;
    }
}

void RoutePlan::Allocate() {
    int day_span = in.get_dayspan();
    plan.resize(in.get_num_vehicle(), std::vector<int>(-1, day_span));
}

int Route::length(const ProbInput& in) const {
    int len = 0;
    std::string client_from(in.get_depot());
    for (unsigned i = 0; i < orders.size(); ++i) {
        std::string client_to = in.OrderVect(orders[i]).get_client();
        len += in.get_distance(client_from, client_to);
        client_from = client_to;
    }
    len += in.get_distance(client_from, in.get_depot());
    return len;
}

int Route::demand(const ProbInput& in) const {
    int demand = 0;
    for (unsigned i = 0; i < orders.size(); ++i) {
        demand += in.OrderVect(orders[i]).get_demand();
    }
    return demand;
}
