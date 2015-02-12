#include <vector>
#include <utility>
#include "helpers/vrp_state_manager.h"

void VRPStateManager::RandomState(RoutePlan& rp) {
    const std::pair &plan_horizon = in.get_plan_horizon();
    bool schduled = false;
    int day, vid;
    std::vector<int> orders;
    assert(plan_horizon.first >= 1);
    for (int i = 0, j = 0; i < in.get_num_order(); ++i) {
        while (j < plan_horizon.second) {
            if (in.OrderVect(i).IsDayFeasible(j)) {
                orders.push_back(i);
                for (int k = 0; k < in.get_num_vehicle(); ++k) {
                    if (in.IsReachable(in.VehicleVect(k), in.OrderVect(i))) {
                        vid = k;
                        schduled = true;
                        break;
                    }
                }
                day = j;
                break;
            }
            ++j;
        }
    }
    if (schduled) 
}
