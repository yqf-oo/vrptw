#include "helpers/vrp_state_manager.h"
#include <utils/Random.hh>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <cassert>
#include "data/order.h"
#include "data/billing.h"
#include "helpers/billing_cost_component.h"

void VRPStateManager::RandomState(RoutePlan &rp) {
    ResetState(rp);
    for (int i = 0; i < in.get_num_ogroup(); ++i) {
        const OrderGroup &o = in.OrderGroupVect(i);
        std::pair<int, int> date_window = o.get_dw();
        assert(date_window.first >= 1);
        int day = Random::Int(date_window.first - 1, date_window.second - 1);
        assert(o.IsDayFeasible(day + 1));

        std::vector<int> rvec(0);
        for (int k = 0; k < in.get_num_vehicle(); ++k)
            if (in.IsReachable(k, i))
                rvec.push_back(k);
        assert(rvec.size() >= 1);
        int idx = Random::Int(0, rvec.size() - 1);
        rp.AddOrder(i, day, rvec[idx], false);
    }
    UpdateTimeTable(rp);
}

void VRPStateManager::ResetState(RoutePlan &rp) {
    for (unsigned i = 0; i < rp.size(); ++i)
        rp[i].clear();
}

void VRPStateManager::UpdateTimeTable(RoutePlan &rp) {
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        std::string client_from(in.get_depot());
        int arrive_time = in.get_depart_time();
        int stop_time = in.get_depart_time();
        int route_size = rp[i].size();

        rp.ResizeRouteTimetable(i, route_size + 1, 0);

        for (int j = 0; j <= route_size; ++j) {
            std::string client_to(in.get_depot());
            if (j < route_size)
                client_to = in.OrderGroupVect(rp[i][j]).get_client();
            int ready_time = in.FindClient(client_to).get_ready_time();
            arrive_time += in.FindClient(client_from).get_service_time()
                           + in.get_time_dist(client_from, client_to);
            if (arrive_time - stop_time > 45 * 360) {    // driving rests
                arrive_time += 45 * 60;
                if (arrive_time < ready_time)
                    arrive_time = ready_time;
                stop_time = arrive_time;
            } else if (arrive_time < ready_time) {
                if (ready_time - arrive_time >= 45 * 60)
                    stop_time = ready_time;
                arrive_time = ready_time;
            }
            rp(i, j) = arrive_time;
            // timetable[i][j] = arrive_time;
            client_from = client_to;
        }
    }
}

int VRPStateManager::CostFunction(const RoutePlan &rp) const {
    return (Objective(rp) + Violations(rp));
}

int VRPStateManager::Objective(const RoutePlan &rp) const {
    int obj = ComputeDateViolationCost(rp, 30) +
                   ComputeTimeViolationCost(rp, 10) +
                   ComputeOptOrderCost(rp, 250) +
                   ComputeTranportationCost(rp);
    return obj;
}

int VRPStateManager::Violations(const RoutePlan &rp) const {
    return ComputeCapExceededCost(rp, 1) + ComputeLateReturnCost(rp, 1);
}

int
VRPStateManager::ComputeDateViolationCost(const RoutePlan& rp,
                                          int weight) const {
    int cost = 0;
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        for (unsigned k = 0; k < rp[i].size(); ++k) {
            const OrderGroup &og = in.OrderGroupVect(rp[i][k]);
            int day = rp[i].get_day() + 1;
            cost += (1 - og.IsDayFeasible(day)) * og.get_demand();
        }
    }
    return (weight * cost);
}

int
VRPStateManager::ComputeTimeViolationCost(const RoutePlan &rp,
                                          int weight) const {
    int cost = 0;
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        for (unsigned j = 0; j < rp[i].size(); ++j) {
            std::string client = in.OrderVect(rp[i][j]).get_client();
            int duetime = in.FindClient(client).get_due_time();
            int tt = rp(i, j);  // get arrive time of order j on route i
            if (tt > duetime)
                cost += tt - duetime;
        }
    }
    return (weight * cost);
}

int
VRPStateManager::ComputeOptOrderCost(const RoutePlan &rp, int weight) const {
    int cost = 0, extra_list = rp.size() - 1;
    for (unsigned i = 0; i < rp[extra_list].size(); ++i) {
        const OrderGroup &og = in.OrderGroupVect(rp[extra_list][i]);
        if (!og.IsMandatory())
            cost += og.get_demand();
    }
    return (weight * cost);
}

int
VRPStateManager::ComputeTranportationCost(const RoutePlan &rp) const {
    int cost = 0;
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        const Billing *cr = in.FindBilling(rp[i].get_vehicle());
        cost += cr->GetCostComponent().Cost(rp[i]);
        if (rp[i].size())   // add vehcile fixed cost
            cost += in.VehicleVect(rp[i].get_vehicle()).fixed_cost();
    }
    return cost;
}

int
VRPStateManager::ComputeCapExceededCost(const RoutePlan &rp, int weight) const {
    int cost = 0;
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        unsigned vehicle_cap = in.VehicleVect(rp[i].get_vehicle()).get_cap();
        unsigned route_demand = rp[i].demand();
        if (route_demand > vehicle_cap)
            cost += route_demand - vehicle_cap;
    }
    return (weight * cost);
}

int
VRPStateManager::ComputeLateReturnCost(const RoutePlan &rp, int weight) const {
    int cost = 0;
    int shutdown_time = in.get_return_time() + 3600;  // plus 1 hour
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        for (unsigned j = 0; j < rp[i].size(); ++j) {
            if (rp(i, j) > shutdown_time)
                ++cost;
        }
    }
    return (weight * cost);
}
