#include <vector>
#include <string>
#include <utility>
#include "data/route.h"

void RoutePlan::AddOrder(int order_index, unsigned day, unsigned vid) {
    int &rid = plan[vid][day];
    if (rid != -1) {
        routes[rid].AddOrder(order_index);
    } else {
        routes.push_back(Route(order_index, day, vid, false));
        rid = routes.size() - 1;
    }
}

void RoutePlan::Allocate() {
    int day_span = in.get_dayspan();
    plan.resize(in.get_num_vehicle(), std::vector<int>(-1, day_span));
}

unsigned Route::length(const ProbInput& in) const {
    int len = 0;
    std::string client_from(in.get_depot());
    for (int i = 0; i < orders.size(); ++i) {
        std::string client_to = in.OrderVect(orders[i]).get_client();
        len += in.get_distance(client_from, client_to);
        client_from = client_to;
    }
    len += in.get_distance(client_from, in.get_depot());
    return len;
}

unsigned Route::demand(const ProbInput& in) const {
    int demand = 0;
    for (int i = 0; i < orders.size(); ++i) {
        demand += in.OrderVect(orders[i]).get_demand();
    }
    return demand;
}
