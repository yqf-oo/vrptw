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

void VRPStateManager::RandomState(RoutePlan& rp) {
    ResetState(rp);
    const std::pair &plan_horizon = in.get_plan_horizon();
    std::vectro<int> unschduled_orders;
    bool unschduled = false;
    assert(plan_horizon.first >= 1);
    for (int i = 0, rnd = 0; i < in.get_num_order(); ++i) {
        int day, vid;
        do {
            day = Random::Int(0, plan_horizon.second - 1);
            if (++rnd > in.get_dayspan()) {
                unschduled = true;
                break;
            }
        }while(!in.OrderVect(i).IsDayFeasible(day));
        rnd = 0;
        if (!unschduled) {
            do {
                vid = Random::Int(0, in.get_num_vehicle() - 1);
                if (++rnd > in.get_num_vehicle()) {
                    unschduled = true;
                    break;
                }
            }while(!in.IsReachable(in.VehicleVect(vid), in.OrderVect(i)));
        }
        if (unschduled)
            rp.AddOrder(i, 0, 0, unschduled);
        else
            unschduled_orders.push_back(i);
    }
    rp.AddRoute(Route(unschduled_orders));
    UpdateTimeTable(rp);
}

void VRPStateManager::ResetState(RoutePlan &rp) {
    for (int i = 0; i < in.get_num_vehicle(); ++i) {
        for (int j = 0; j < in.get_dayspan(); ++j) {
            rp(i, j) = -1;
        }
    }
}

void VRPStateManager::UpdateTimeTable(RoutePlan &rp) {
    timetable.resize(rp.size());
    for (int i = 0; i < rp.size(); ++i) {
        std::string client_from(in.get_depot());
        int arrive_time = in.get_depart_time();
        int stop_time = in.get_depart_time();
        int route_size = rp[i].size();

        timetable[i].resize(route_size + 1, 0);

        for (int j = 0; j <= route_size; ++j) {
            std::string client_to(in.get_depot());
            if (j < route_size)
                client_to = in.OrderVect(rp[i][j]).get_client();
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
            timetable[i][j] = arrive_time;
        }
    }
}

unsigned VRPStateManager::CostFunction(const RoutePlan &rp) const {
    return (Objective(rp) + Violations(rp));
}

unsigned VRPStateManager::Objective(const RoutePlan &rp) const {
    unsigned obj = ComputeDateViolationCost(rp) + ComputeTimeViolationCost(rp) +
              ComputeOptOrderCost(rp) + ComputeTranportationCost(rp);
    return obj;
}

unsigned VRPStateManager::Violations(const RoutePlan &rp) const {
    return ComputeCapExceededCost(rp) + ComputeLateReturnCost(rp);
}

unsigned
VRPStateManager::ComputeDateViolationCost(const RoutePlan& rp,
                                          int weight) const {
    int num_vehicle = in.get_num_vehicle();
    unsigned day_span = in.get_dayspan(), cost = 0;
    for (int i = 0; i < num_vehicle; ++i) {
        for (unsigned j = 0; j < day_span; ++j) {
            int rid = rp(i, j);
            if (rid == -1)
                continue;
            for (int k = 0; k < rp[rid].size(); ++k) {
                int oid = rp[rid][k];
                const Order& ord = in.OrderVect(oid);
                cost += (1 - ord.IsDayFeasible(j + 1)) * ord.get_demand();
            }
        }
    }
    return (weight * cost);
}

unsigned
VRPStateManager::ComputeTimeViolationCost(const RoutePlan &rp,
                                          int weight) const {
    int cost = 0;
    for (int i = 0; i < rp.size(); ++i) {
        for (int j = 0; j < rp[i].size(); ++j) {
            std::string client = in.OrderVect(rp[i][j]).get_client();
            int duetime = in.FindClient(client).get_due_time();
            if (timetable[i][j] > duetime)
                cost += timetable[i][j] - duetime;
        }
    }
    return (weight * cost);
}

unsigned
VRPStateManager::ComputeOptOrderCost(const RoutePlan &rp, int weight) const {
    unsigned cost = 0;
    for (int i = 0; i < rp.size(); ++i) {
        if (rp[i].IsExcList()) {
            for (int j = 0; j < rp[i].size(); ++j) {
                const Order& ord = in.OrderVect(rp[i][j]);
                if (!ord.IsMandatory())
                    cost += ord.get_demand();
            }
        }
    }
    return (weight * cost);
}

unsigned
VRPStateManager::ComputeTranportationCost(const RoutePlan &rp) const {
    unsigned cost = 0;
    for (unsigned i = 0; i < rp.size(); ++i) {
        if (!rp[i].IsExcList) {
            Billing *cr = in.FindBilling(rp[i].get_vehicle());
            cost += cr->GetCostComponent().Cost(rp[i]);
        }
    }
    return cost;
}

unsigned
VRPStateManager::ComputeCapExceededCost(const RoutePlan &rp, int weight) const {
    unsigned cost = 0;
    for (unsigned i = 0; i < rp.size(); ++i) {
        if (rp[i].IsExcList())
            continue;
        unsigned vehicle_cap = in.VehicleVect(rp[i].get_vehicle()).get_cap();
        unsigned route_demand = rp[i].demand(in);
        if (route_demand > vehicle_cap)
            cost += route_demand - vehicle_cap;
    }
    return (weight * cost);
}

unsigned
VRPStateManager::ComputeLateReturnCost(const RoutePlan &rp, int weight) const {
    unsigned cost = 0;
    unsigned shutdown_time = in.get_return_time() + 3600;  // plus 1 hour
    for (unsigned i = 0; i < rp.size(); ++i) {
        if (!rp[i].IsExcList()) {
            for (unsigned j = 0; j < rp[i].size(); ++j) {
                if (timetable[i][j] > shutdown_time)
                    ++cost;
            }
        }
    }
    return (weight * cost);
}
