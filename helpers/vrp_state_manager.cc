#include <vector>
#include <utility>
#include "helpers/vrp_state_manager.h"
#include <utils/Random.hh>

void VRPStateManager::RandomState(RoutePlan& rp) {
    const std::pair &plan_horizon = in.get_plan_horizon();
    bool schduled = false;
    int day, vid;
    std::vector<int> orders;
    assert(plan_horizon.first >= 1);
    for (int i = 0; i < in.get_num_order(); ++i) {
        do {
            day = Random::Int(0, plan_horizon.second - 1);
        }while(!in.OrderVect(i).IsDayFeasible(day));
        do {
            vid = Random::Int(0, in.get_num_vehicle() - 1);
        }while(!in.IsReachable(in.VehicleVect(vid), in.OrderVect(i)));
    }
}
