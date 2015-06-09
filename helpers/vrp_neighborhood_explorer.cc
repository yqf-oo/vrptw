#include "helpers/vrp_neighborhood_explorer.h"
#include <utils/Random.hh>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cassert>
#include "data/billing.h"
#include "helpers/billing_cost_component.h"

void InsMoveNeighborhoodExplorer::RandomMove(const RoutePlan &rp,
                                             InsMove &mv) const {
    do {
        AnyRandomMove(rp, mv);
    }while (!FeasibleMove(rp, mv));
}

bool InsMoveNeighborhoodExplorer::FeasibleMove(const RoutePlan &rp,
                                               const InsMove &mv) const {
    // std::ofstream f("./logs/fea.move", std::ios::app);
    // f << mv << std::endl;
    if (!rp[mv.old_route].size())   // old route is null
        return false;
    // if (mv.old_pos > rp[mv.old_route].size() - 1 ||
    //     mv.new_pos > rp[mv.new_route].size())
    //     return false;
    if (mv.new_route == mv.old_route)
        return false;
    if (rp[mv.new_route].IsExcList()) {
        if (in.OrderGroupVect(mv.order).IsMandatory())
            return false;
    } else {
        // f << "here " << mv << std::endl;
        int vehicle = rp[mv.new_route].get_vehicle();
        unsigned vehicle_cap = in.VehicleVect(vehicle).get_cap();
        unsigned order_demand = in.OrderGroupVect(mv.order).get_demand();
        if (order_demand + rp[mv.new_route].demand() > vehicle_cap)
            return false;
    }
    return true;
}

void InsMoveNeighborhoodExplorer::AnyRandomMove(const RoutePlan &rp,
                                                InsMove &mv) const {
    mv.old_route = Random::Int(0, rp.size() - 1);
    mv.old_pos = mv.order = 0;
    if (rp[mv.old_route].size()) {
        mv.old_pos = Random::Int(0, rp[mv.old_route].size() - 1);
        mv.order = rp[mv.old_route][mv.old_pos];
    }
    mv.new_route = Random::Int(0, rp.size() - 1);
    mv.new_pos = 0;     // for null routes
    if (rp[mv.new_route].size())
        mv.new_pos = Random::Int(0, rp[mv.new_route].size());
}

void InsMoveNeighborhoodExplorer::FirstMove(const RoutePlan &rp,
                                            InsMove &mv) const {
    for (unsigned i = 0; i < rp.size(); ++i) {
        if (rp[i].size()) {
            mv.old_route = i;
            break;
        }
    }
    mv.old_pos = 0;
    mv.order = rp[mv.old_route][mv.old_pos];
    mv.new_route = mv.old_route + 1;
    mv.new_pos = 0;
    if (!FeasibleMove(rp, mv))
        NextMove(rp, mv);
}

bool InsMoveNeighborhoodExplorer::NextMove(const RoutePlan &rp,
                                           InsMove &mv) const {
    bool not_last = true;
    do {
        not_last = AnyNextMove(rp, mv);
    }while(!FeasibleMove(rp, mv) && not_last);
    return not_last;
}

bool InsMoveNeighborhoodExplorer::AnyNextMove(const RoutePlan &rp,
                                              InsMove &mv) const {
    unsigned old_route_size = rp[mv.old_route].size();
    unsigned new_route_size = rp[mv.new_route].size();
#ifdef _DEBUG_H_
    std::ofstream f("./logs/move.log", std::ios::app);
    f << "-- " << "size:" << new_route_size << " " << mv << " --" << std::endl;
#endif
    if (new_route_size && mv.new_pos < new_route_size) {
        mv.new_pos++;
    } else if (mv.new_route < rp.size() - 1) {
        mv.new_route++;
        mv.new_pos = 0;
    } else {
        if (old_route_size && mv.old_pos < old_route_size - 1) {
            mv.old_pos++;
        } else if (mv.old_route < rp.size() - 1) {
            mv.old_route++;
            mv.old_pos = 0;
        } else {
            return false;
        }
        // if (!rp[mv.old_route].size())
        //     mv.order = rp[mv.old_route][mv.old_pos];
        mv.new_route = 0;
        mv.new_pos = 0;
    }
    if  (rp[mv.old_route].size())
        mv.order = rp[mv.old_route][mv.old_pos];
    return true;
}

void
InsMoveNeighborhoodExplorer::MakeMove(RoutePlan &rp,
                                      const InsMove &mv) const {
    rp[mv.old_route].erase(mv.old_pos);
    rp[mv.new_route].insert(mv.new_pos, mv.order);
    // update timetable
    if (!rp[mv.old_route].IsExcList())
        UpdateRouteTimetable(rp.timetable(mv.old_route), rp[mv.old_route]);
    if (!rp[mv.new_route].IsExcList())
        UpdateRouteTimetable(rp.timetable(mv.new_route), rp[mv.new_route]);
}

int
InsMoveNeighborhoodExplorer::DeltaCostFunction(const RoutePlan &rp,
                                               const InsMove &mv) const {
    // ugly reduntant data
    #ifdef _DEBUG_H_
    std::fstream f("./logs/error.log", std::ios::app);
    f << in.OrderGroupVect(mv.order) << std::endl;
    f << "-- " << std::endl << rp << std::endl << mv << std::endl
      << "-- " << std::endl;
    #endif
    routes_.clear();
    routes_.push_back(rp[mv.old_route]);
    routes_.push_back(rp[mv.new_route]);
    routes_[0].erase(mv.old_pos);
    routes_[1].insert(mv.new_pos, mv.order);
    timetable_.clear();
    timetable_.resize(2);
    if (!routes_[0].IsExcList())
        UpdateRouteTimetable(timetable_[0], routes_[0]);
    if (!routes_[1].IsExcList())
        UpdateRouteTimetable(timetable_[1], routes_[1]);
    delta_num_order_late_return = 0;

    return DeltaObjective(rp, mv) + DeltaViolations(rp, mv);
}

int
InsMoveNeighborhoodExplorer::DeltaObjective(const RoutePlan &rp,
                                            const InsMove &mv) const {
    // int date_delta = DeltaDateViolationCost(rp, mv, 30);
    // int time_delta = DeltaTimeViolationCost(rp, mv, 10);
    // int opt_delta = DeltaOptOrderCost(rp, mv, 250);
    // int trans_delta = DeltaTranportationCost(rp, mv);
    // std::cout << "delta objective:";
    // std:: cout << date_delta << ", " << time_delta << ", "
    //            << opt_delta << ", " << trans_delta << std::endl;
    // return date_delta + time_delta + opt_delta + trans_delta;
    return DeltaDateViolationCost(rp, mv, 30)
           + DeltaTimeViolationCost(rp, mv, 10)
           + DeltaOptOrderCost(rp, mv, 250)
           + DeltaTranportationCost(rp, mv);
}

int
InsMoveNeighborhoodExplorer::DeltaViolations(const RoutePlan &rp,
                                             const InsMove &mv) const {
    #ifdef _NE_DEBUG_H_
    int cap_delta = DeltaCapExceededCost(rp, mv, 1);
    int late_delta = DeltaLateReturnCost(rp, mv, 1);
    if (cap_delta) {
        std::cout << "---" << std::endl;
        std::cout << rp.get_vio() << std::endl;
        std::cout << rp << std::endl;
        std::cout << mv << std::endl;
        std::cout << "cap: " << cap_delta << ", " << "late: " << late_delta << std::endl;
        std::cout << rp[mv.new_route].demand() << ", "
                  << in.VehicleVect(rp[mv.new_route].get_vehicle()).get_cap() << ", "
                  << in.OrderGroupVect(mv.order).get_demand() << std::endl;
        std::cout << in.OrderGroupVect(mv.order) << std::endl;
    }
    return cap_delta + late_delta;
    #else
    return DeltaCapExceededCost(rp, mv, 1) + DeltaLateReturnCost(rp, mv, this->vio_wt);
    #endif
}

int
InsMoveNeighborhoodExplorer::DeltaDateViolationCost(const RoutePlan &rp,
                                        const InsMove &mv, int weight) const {
    int delta = 0;
    const OrderGroup& o = in.OrderGroupVect(mv.order);
    unsigned og_demand = o.get_demand();
    if (!rp[mv.old_route].IsExcList()) {
        unsigned old_day = rp[mv.old_route].get_day();
        delta -= (1 - o.IsDayFeasible(old_day)) * og_demand;
    }
    if (!rp[mv.new_route].IsExcList()) {
        unsigned new_day = rp[mv.new_route].get_day();
        delta += (1 - o.IsDayFeasible(new_day)) * og_demand;
    }
    return (weight * delta);
}

int
InsMoveNeighborhoodExplorer::DeltaTimeViolationCost(const RoutePlan &rp,
                                        const InsMove &mv, int weight) const {
    int delta = 0;
    int late_ret_bef = 0, late_ret_aft = 0;
    if (!rp[mv.old_route].IsExcList()) {
        int cost_bef = RouteCostsOnTimeWindow(rp[mv.old_route],
                                              rp.timetable(mv.old_route),
                                              &late_ret_bef);
        int cost_aft = RouteCostsOnTimeWindow(routes_[0],
                                              timetable_[0],
                                              &late_ret_aft);
        delta += cost_aft - cost_bef;
        delta_num_order_late_return += late_ret_aft - late_ret_bef;
    }

    if (!rp[mv.new_route].IsExcList()) {
        int cost_bef = RouteCostsOnTimeWindow(rp[mv.new_route],
                                              rp.timetable(mv.new_route),
                                              &late_ret_bef);
        int cost_aft = RouteCostsOnTimeWindow(routes_[1],
                                              timetable_[1],
                                              &late_ret_aft);
        delta += cost_aft - cost_bef;
        delta_num_order_late_return += late_ret_aft - late_ret_bef;
    }

    return (weight * delta);
}

// int
// InsMoveNeighborhoodExplorer::DeltaRouteTimeViolation(const RoutePlan &rp,
//                                   int isnew, unsigned route, unsigned pos) {
//     int delta = 0;
//     for (int i = pos; i < rp[route].size(); ++i) {
//         client = in.OrderVect(rp[route][i]).get_client();
//         int duetime = in.FindClient(client).get_due_time();
//         int arrive_time = rp(route, i);
//         if (arrive_time > duetime)
//             delta -= arrive_time - duetime;
//     }
//         // after move
//     for (int i = pos; i < routes_[isnew].size(); ++i) {
//         client = in.OrderVect(routes_[isnew][i]).get_client();
//         int duetime = in.FindClient(client).get_due_time();
//         if (timetable[isnew][i] > duetime)
//             delta += timetable[isnew][i] - duetime;
//     }
//     return delta;
// }

int
InsMoveNeighborhoodExplorer::DeltaOptOrderCost(const RoutePlan &rp,
                                        const InsMove &mv, int weight) const {
    int delta = 0;
    const OrderGroup& o = in.OrderGroupVect(mv.order);
    if (!o.IsMandatory()) {
        if (rp[mv.old_route].IsExcList())
            delta -= o.get_demand();
        else if (rp[mv.new_route].IsExcList())
            delta += o.get_demand();
    }
    return (weight * delta);
}

int
InsMoveNeighborhoodExplorer::DeltaTranportationCost(const RoutePlan &rp,
                                                    const InsMove &mv) const {
    int delta = 0;
    const Billing *cr = in.FindBilling(rp[mv.old_route].get_vehicle());
    if (!rp[mv.old_route].IsExcList()) {
        delta += cr->GetCostComponent().Cost(routes_[0]) -
                 cr->GetCostComponent().Cost(rp[mv.old_route]);
        const Vehicle &v = in.VehicleVect(rp[mv.old_route].get_vehicle());
        if (!routes_[0].size())
            delta -= v.fixed_cost();
    }

    cr = in.FindBilling(rp[mv.new_route].get_vehicle());
    if (!rp[mv.new_route].IsExcList()) {
        delta += cr->GetCostComponent().Cost(routes_[1]) -
                 cr->GetCostComponent().Cost(rp[mv.new_route]);
        const Vehicle &v = in.VehicleVect(rp[mv.new_route].get_vehicle());
        if (!rp[mv.new_route].size())
            delta += v.fixed_cost();
    }

    return delta;
}

int
InsMoveNeighborhoodExplorer::DeltaCapExceededCost(const RoutePlan &rp,
                                        const InsMove &mv, int weight) const {
    int delta = 0;
    unsigned vehicle_cap = 0, route_demand = 0;
    unsigned order_demand = in.OrderGroupVect(mv.order).get_demand();
    if (!rp[mv.old_route].IsExcList()) {
        vehicle_cap = in.VehicleVect(rp[mv.old_route].get_vehicle()).get_cap();
        route_demand = rp[mv.old_route].demand();
        if (route_demand > vehicle_cap) {
            if (route_demand - order_demand > vehicle_cap)
                delta -= order_demand;
            else
                delta -= route_demand - vehicle_cap;
        }
    }
    if (!rp[mv.new_route].IsExcList()) {
        vehicle_cap = in.VehicleVect(rp[mv.new_route].get_vehicle()).get_cap();
        route_demand = rp[mv.new_route].demand();
        if (route_demand > vehicle_cap)
            delta += order_demand;
        else if (route_demand + order_demand > vehicle_cap)
            delta += route_demand + order_demand - vehicle_cap;
    }
    delta_cap = delta;
    return (weight * delta);
}

int
InsMoveNeighborhoodExplorer::DeltaLateReturnCost(const RoutePlan &rp,
                                        const InsMove &mv, int weight) const {
    int delta = delta_num_order_late_return;
    // int shutdown_time = in.get_return_time() + 3600;  // plus 1 hour
    // if (!rp[mv.old_route].IsExcList()) {
    //     if (rp(mv.old_route, mv.old_pos) > shutdown_time)
    //         --delta;
    //     for (unsigned j = mv.old_pos; j < routes_[0].size(); ++j) {
    //         if (rp(mv.old_route, j + 1) > shutdown_time)
    //             --delta;
    //         if (timetable_[0][j] > shutdown_time)
    //             ++delta;
    //     }
    // }
    // // if (mv.new_pos >= timetable[1].size())
    // //     std::cout << mv << std::endl;
    // // assert(mv.new_pos < timetable[1].size());
    // if (!rp[mv.new_route].IsExcList()) {
    //     if (timetable_[1][mv.new_pos] > shutdown_time)
    //         ++delta;
    //     for (unsigned j = mv.new_pos; j < rp[mv.new_route].size(); ++j) {
    //         if (rp(mv.new_route, j) > shutdown_time)
    //             --delta;
    //         if (timetable_[1][j + 1] > shutdown_time)
    //             ++delta;
    //     }
    // }
    return (weight * delta);
}


// Inter Route Swap Implementation

void InterSwapNeighborhoodExplorer::RandomMove(const RoutePlan &rp,
                                             InterSwap &mv) const {
    do {
        AnyRandomMove(rp, mv);
    }while (!FeasibleMove(rp, mv));
}

bool InterSwapNeighborhoodExplorer::FeasibleMove(const RoutePlan &rp,
                                               const InterSwap &mv) const {
#ifdef _VNE_DEBUG_H_
    std::ofstream f("./logs/fea.move", std::ios::app);
    f << mv << std::endl;
#endif
    if (!rp[mv.route1].size() || !rp[mv.route2].size())
        return false;
    if (rp[mv.route1].IsExcList() &&
        in.OrderGroupVect(mv.ord2).IsMandatory())
        return false;
    else if (rp[mv.route2].IsExcList() &&
             in.OrderGroupVect(mv.ord1).IsMandatory())
        return false;
    if (mv.route1 == mv.route2)
        return false;
    unsigned d2 = in.OrderGroupVect(mv.ord2).get_demand();
    unsigned d1 = in.OrderGroupVect(mv.ord1).get_demand();
    int old_veh = rp[mv.route1].get_vehicle();
    unsigned old_cap = in.VehicleVect(old_veh).get_cap();
    int new_veh = rp[mv.route2].get_vehicle();
    unsigned new_cap = in.VehicleVect(new_veh).get_cap();
    if (!rp[mv.route1].IsExcList() && !rp[mv.route2].IsExcList()) {
#ifdef _VNE_DEBUG_H_
        f << "#1 " << mv << std::endl;
        f << "--" << std::endl;
        f << "r1: " << old_veh << ", " << old_cap << ", " << d1 << ", " << rp[mv.route1].demand() << std::endl;
        f << "r2: " << new_veh << ", " << new_cap << ", " << d2 << ", " << rp[mv.route2].demand() << std::endl;
        f << "--" << std::endl;
#endif
        if (d2 + rp[mv.route1].demand() - d1 > old_cap)
            return false;
        else if (d1 + rp[mv.route2].demand() - d2> new_cap)
            return false;
    } else {
        if (!rp[mv.route1].IsExcList()) {
#ifdef _VNE_DEBUG_H_
            f << "#2 " << mv << std::endl;
#endif
            if (d2 + rp[mv.route1].demand() - d1 > old_cap)
                return false;
        } else if (!rp[mv.route2].IsExcList()) {
#ifdef _VNE_DEBUG_H_
            f << "#2 " << mv << std::endl;
#endif
            if (d1 + rp[mv.route2].demand() - d2 > new_cap)
                return false;
        }
    }
    return true;
}

void InterSwapNeighborhoodExplorer::AnyRandomMove(const RoutePlan &rp,
                                                InterSwap &mv) const {
    mv.route1 = Random::Int(0, rp.size() - 1);
    mv.pos1 = mv.ord1 = 0;
    if (rp[mv.route1].size()) {
        mv.pos1 = Random::Int(0, rp[mv.route1].size() - 1);
        mv.ord1 = rp[mv.route1][mv.pos1];
    }
    mv.route2 = Random::Int(0, rp.size() - 1);
    mv.pos2 = mv.ord2 = 0;
    if (rp[mv.route2].size()) {
        mv.pos2 = Random::Int(0, rp[mv.route2].size() - 1);
        mv.ord2 = rp[mv.route2][mv.pos2];
    }
}

void InterSwapNeighborhoodExplorer::FirstMove(const RoutePlan &rp,
                                            InterSwap &mv) const {
    // assert(rp.size() > 1);
    mv.route1 = mv.route2 = 0;
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        if (rp[i].size()) {
            mv.route1 = i;
            break;
        }
    }
    for (unsigned i = mv.route1 + 1; i < rp.num_routes(); ++i) {
        if (rp[i].size()) {
            mv.route2 = i;
            break;
        }
    }
    if (!rp[mv.route1].size() || !rp[mv.route2].size())
        std::cout << "route: " << mv.route1 << ", " << mv.route2 << std::endl;
    assert(rp[mv.route1].size() > 0 && rp[mv.route2].size() > 0);
    mv.pos1 = 0;
    mv.ord1 = rp[mv.route1][mv.pos1];
    mv.pos2 = 0;
    mv.ord2 = rp[mv.route2][mv.pos2];
    if (!FeasibleMove(rp, mv))
        NextMove(rp, mv);
}

bool InterSwapNeighborhoodExplorer::NextMove(const RoutePlan &rp,
                                           InterSwap &mv) const {
    bool not_last = true;
    do {
        not_last = AnyNextMove(rp, mv);
    }while(!FeasibleMove(rp, mv) && not_last);
#ifdef _VNE_DEBUG_H_
    std::ofstream f("./logs/next.move", std::ios::app);
    f << mv << std::endl;
#endif
    return not_last;
}

bool InterSwapNeighborhoodExplorer::AnyNextMove(const RoutePlan &rp,
                                              InterSwap &mv) const {
    unsigned route1_size = rp[mv.route1].size();
    unsigned route2_size = rp[mv.route2].size();
#ifdef _VNE_DEBUG_H_
    std::ofstream f("./logs/move.log", std::ios::app);
    f << mv << std::endl;
#endif
    #ifdef _DEBUG_H_
    std::fstream f("./logs/move.log", std::ios::app);
    f << "-- " << "size:" << route1_size << ", " << route2_size
      << " " << mv << " --" << std::endl;
    #endif
    if (route2_size && mv.pos2 < route2_size - 1) {
        mv.pos2++;
    } else if (mv.route2 < rp.size() - 1) {
        mv.route2++;
        mv.pos2 = 0;
    } else {
        if (route1_size && mv.pos1 < rp[mv.route1].size() - 1) {
            mv.pos1++;
            mv.route2 = mv.route1 + 1;
            mv.pos2 = 0;
        } else if (mv.route1 < rp.size() - 2) {
            mv.route1++;
            mv.pos1 = 0;
            mv.route2 = mv.route1 + 1;
            mv.pos2 = 0;
        } else {
            return false;   // last move in state
        }
    }
    if (rp[mv.route1].size())
        mv.ord1 = rp[mv.route1][mv.pos1];
    if (rp[mv.route2].size())
        mv.ord2 = rp[mv.route2][mv.pos2];
    return true;
}

void
InterSwapNeighborhoodExplorer::MakeMove(RoutePlan &rp,
                                      const InterSwap &mv) const {
    std::swap(rp[mv.route1][mv.pos1], rp[mv.route2][mv.pos2]);
    // update timetable
    if (!rp[mv.route1].IsExcList())
        UpdateRouteTimetable(rp.timetable(mv.route1), rp[mv.route1]);
    if (!rp[mv.route2].IsExcList())
        UpdateRouteTimetable(rp.timetable(mv.route2), rp[mv.route2]);
}

int
InterSwapNeighborhoodExplorer::DeltaCostFunction(const RoutePlan &rp,
                                               const InterSwap &mv) const {
    // ugly reduntant data
    routes_.clear();
    routes_.push_back(rp[mv.route1]);
    routes_.push_back(rp[mv.route2]);
    std::swap(routes_[0][mv.pos1], routes_[1][mv.pos2]);
    timetable_.clear();
    timetable_.resize(2);
    if (!routes_[0].IsExcList())
        UpdateRouteTimetable(timetable_[0], routes_[0]);
    if (!routes_[1].IsExcList())
        UpdateRouteTimetable(timetable_[1], routes_[1]);

    delta_num_order_late_return = 0;
    return DeltaObjective(rp, mv) + DeltaViolations(rp, mv);
}

int
InterSwapNeighborhoodExplorer::DeltaObjective(const RoutePlan &rp,
                                            const InterSwap &mv) const {
    #ifdef _NE_DEBUG_H_
    int date_delta = DeltaDateViolationCost(rp, mv, 30);
    int time_delta = DeltaTimeViolationCost(rp, mv, 10);
    int opt_delta = DeltaOptOrderCost(rp, mv, 250);
    int trans_delta = DeltaTranportationCost(rp, mv);
    std::cout << "delta objective:";
    std:: cout << date_delta << ", " << time_delta << ", "
               << opt_delta << ", " << trans_delta << std::endl;
    return date_delta + time_delta + opt_delta + trans_delta;
    #else
    return DeltaDateViolationCost(rp, mv, 30)
           + DeltaTimeViolationCost(rp, mv, 10)
           + DeltaOptOrderCost(rp, mv, 250)
           + DeltaTranportationCost(rp, mv);
    #endif
}

int
InterSwapNeighborhoodExplorer::DeltaViolations(const RoutePlan &rp,
                                             const InterSwap &mv) const {
    #ifdef _NE_DEBUG_H_
    int cap_delta = DeltaCapExceededCost(rp, mv, 1);
    int late_delta = DeltaLateReturnCost(rp, mv, 1);
    std::cout << "---" << std::endl;
    std::cout << rp.get_vio() << std::endl;
    std::cout << rp << std::endl;
    std::cout << mv << std::endl;
    std::cout << "cap: " << cap_delta << ", " << "late: " << late_delta << std::endl;
    return cap_delta + late_delta;
    #else
    return DeltaCapExceededCost(rp, mv, 1) + DeltaLateReturnCost(rp, mv, this->vio_wt);
    #endif
}

int
InterSwapNeighborhoodExplorer::DeltaDateViolationCost(const RoutePlan &rp,
                                        const InterSwap &mv, int weight) const {
    int delta = 0;
    const OrderGroup& ord1 = in.OrderGroupVect(mv.ord1);
    const OrderGroup& ord2 = in.OrderGroupVect(mv.ord2);
    unsigned old_day = rp[mv.route1].get_day();
    unsigned new_day = rp[mv.route2].get_day();
    int ord1_demand = ord1.get_demand(), ord2_demand = ord2.get_demand();
    if (!rp[mv.route1].IsExcList()) {
        delta -= (1 - ord1.IsDayFeasible(old_day)) * ord1_demand;
        delta += (1 - ord2.IsDayFeasible(old_day)) * ord2_demand;
    }
    if (!rp[mv.route2].IsExcList()) {
        delta -= (1 - ord2.IsDayFeasible(new_day)) * ord2_demand;
        delta += (1 - ord1.IsDayFeasible(new_day)) * ord1_demand;
    }
    return (weight * delta);
}

int
InterSwapNeighborhoodExplorer::DeltaTimeViolationCost(const RoutePlan &rp,
                                        const InterSwap &mv, int weight) const {
    int delta = 0;
    int late_ret_bef = 0, late_ret_aft = 0;
    if (!rp[mv.route1].IsExcList()) {
        int cost_bef = RouteCostsOnTimeWindow(rp[mv.route1],
                                              rp.timetable(mv.route1),
                                              &late_ret_bef);
        int cost_aft = RouteCostsOnTimeWindow(routes_[0],
                                              timetable_[0],
                                              &late_ret_aft);
        delta += cost_aft - cost_bef;
        delta_num_order_late_return += late_ret_aft - late_ret_bef;
    }

    if (!rp[mv.route2].IsExcList()) {
        int cost_bef = RouteCostsOnTimeWindow(rp[mv.route2],
                                              rp.timetable(mv.route2),
                                              &late_ret_bef);
        int cost_aft = RouteCostsOnTimeWindow(routes_[1],
                                              timetable_[1],
                                              &late_ret_aft);
        delta += cost_aft - cost_bef;
        delta_num_order_late_return += late_ret_aft - late_ret_bef;
    }
    // if (!rp[mv.route1].IsExcList()) {
    //     delta += DeltaRouteTimeViolation(rp, isnew, mv.route1, mv.pos1);
    // }

    // isnew = 1;
    // if (!rp[mv.route2].IsExcList())
    //     delta += DeltaRouteTimeViolation(rp, isnew, mv.route2, mv.pos2);

    return (weight * delta);
}

int
InterSwapNeighborhoodExplorer::DeltaOptOrderCost(const RoutePlan &rp,
                                        const InterSwap &mv, int weight) const {
    int delta = 0;
    const OrderGroup& ord1 = in.OrderGroupVect(mv.ord1);
    const OrderGroup& ord2 = in.OrderGroupVect(mv.ord2);
    if (!ord1.IsMandatory()) {
        if (rp[mv.route1].IsExcList())
            delta -= ord1.get_demand();
        else if (rp[mv.route2].IsExcList())
            delta += ord1.get_demand();
    }
    if (!ord2.IsMandatory()) {
        if (rp[mv.route2].IsExcList())
            delta -= ord2.get_demand();
        else if (rp[mv.route1].IsExcList())
            delta += ord2.get_demand();
    }
    return (weight * delta);
}

int
InterSwapNeighborhoodExplorer::DeltaTranportationCost(const RoutePlan &rp,
                                                    const InterSwap &mv) const {
    int delta = 0;
    const Billing *cr = in.FindBilling(rp[mv.route1].get_vehicle());
    if (!rp[mv.route1].IsExcList())
        delta += cr->GetCostComponent().Cost(routes_[0]) -
                 cr->GetCostComponent().Cost(rp[mv.route1]);
    cr = in.FindBilling(rp[mv.route2].get_vehicle());
    if (!rp[mv.route2].IsExcList())
        delta += cr->GetCostComponent().Cost(routes_[1]) -
                 cr->GetCostComponent().Cost(rp[mv.route2]);
    return delta;
}

int
InterSwapNeighborhoodExplorer::DeltaCapExceededCost(const RoutePlan &rp,
                                        const InterSwap &mv, int weight) const {
    int delta = 0;
    unsigned vehicle_cap = 0, route_demand = 0;
    unsigned demand_from = in.OrderGroupVect(mv.ord1).get_demand();
    unsigned demand_to = in.OrderGroupVect(mv.ord2).get_demand();
    if (!rp[mv.route1].IsExcList()) {
        vehicle_cap = in.VehicleVect(rp[mv.route1].get_vehicle()).get_cap();
        route_demand = rp[mv.route1].demand();
        if (route_demand > vehicle_cap) {
            if (route_demand + demand_to - demand_from > vehicle_cap)
                delta += demand_to - demand_from;
            else
                delta -= route_demand - vehicle_cap;
        } else if (route_demand + demand_to - demand_from > vehicle_cap) {
            delta += route_demand + demand_to - demand_from - vehicle_cap;
        }
    }
    if (!rp[mv.route2].IsExcList()) {
        vehicle_cap = in.VehicleVect(rp[mv.route2].get_vehicle()).get_cap();
        route_demand = rp[mv.route2].demand();
        if (route_demand > vehicle_cap) {
            if (route_demand + demand_from - demand_to > vehicle_cap)
                delta += demand_from - demand_to;
            else
                delta -= route_demand - vehicle_cap;
        } else if (route_demand + demand_from - demand_to > vehicle_cap) {
            delta += route_demand - demand_to + demand_from - vehicle_cap;
        }
    }
    delta_cap = delta;
    return (weight * delta);
}

int
InterSwapNeighborhoodExplorer::DeltaLateReturnCost(const RoutePlan &rp,
                                        const InterSwap &mv, int weight) const {
    int delta = delta_num_order_late_return;
    // int shutdown_time = in.get_return_time() + 3600;  // plus 1 hour

    // if (!rp[mv.route1].IsExcList()) {
    //     for (unsigned j = mv.pos1; j < rp[mv.route1].size(); ++j) {
    //         if (rp(mv.route1, j) > shutdown_time)
    //             --delta;
    //         if (timetable_[0][j] > shutdown_time)
    //             ++delta;
    //     }
    // }

    // if (!rp[mv.route2].IsExcList()) {
    //     for (unsigned j = mv.pos2; j < rp[mv.route2].size(); ++j) {
    //         if (rp(mv.route2, j) > shutdown_time)
    //             --delta;
    //         if (timetable_[1][j] > shutdown_time)
    //             ++delta;
    //     }
    // }

    return (weight * delta);
}


// Intra Route Swap Implementation

void IntraSwapNeighborhoodExplorer::RandomMove(const RoutePlan &rp,
                                             IntraSwap &mv) const {
    do {
        AnyRandomMove(rp, mv);
    }while (!FeasibleMove(rp, mv));
}

bool IntraSwapNeighborhoodExplorer::FeasibleMove(const RoutePlan &rp,
                                               const IntraSwap &mv) const {
    if (rp[mv.route].IsExcList())
        return false;
    if (rp[mv.route].size() < 2)
        return false;
    if (mv.pos1 == mv.pos2)
        return false;
    return true;
}

void IntraSwapNeighborhoodExplorer::AnyRandomMove(const RoutePlan &rp,
                                                IntraSwap &mv) const {
    mv.route = Random::Int(0, rp.size() - 1);
    if (rp[mv.route].size() < 2) {
        mv.pos1 = mv.pos2 = 0;
        mv.ord1 = mv.ord2 = 0;
        return;
    }
    mv.pos1 = Random::Int(0, rp[mv.route].size() - 1);
    mv.pos2 = Random::Int(0, rp[mv.route].size() - 1);
    mv.ord1 = rp[mv.route][mv.pos1];
    mv.ord2 = rp[mv.route][mv.pos2];
    return;
}

void IntraSwapNeighborhoodExplorer::FirstMove(const RoutePlan &rp,
                                            IntraSwap &mv) const {
    assert(rp.size() > 1);
    for (unsigned i = 0; i < rp.num_routes(); ++i) {
        if (rp[i].size() > 2) {
            mv.route = i;
            break;
        }
    }
    mv.pos1 = 0;
    mv.pos2 = 1;
    mv.ord1 = rp[mv.route][mv.pos1];
    mv.ord2 = rp[mv.route][mv.pos2];
    if (!FeasibleMove(rp, mv))
        NextMove(rp, mv);
}

bool IntraSwapNeighborhoodExplorer::NextMove(const RoutePlan &rp,
                                           IntraSwap &mv) const {
    bool not_last = true;
    do {
       not_last = AnyNextMove(rp, mv);
    }while(!FeasibleMove(rp, mv) && not_last);
    if (not_last) {
        mv.ord1 = rp[mv.route][mv.pos1];
        mv.ord2 = rp[mv.route][mv.pos2];
    }
    return not_last;
}

bool IntraSwapNeighborhoodExplorer::AnyNextMove(const RoutePlan &rp,
                                              IntraSwap &mv) const {
    #ifdef _DEBUG_H_
    std::fstream f("./logs/move.log", std::ios::app);
    f << "-- " << mv << " --" << std::endl;
    #endif
    unsigned route_size = rp[mv.route].size();
    if (route_size > 1 && mv.pos2 < route_size - 1) {
        mv.pos2++;
    } else if (route_size > 1 && mv.pos1 < route_size - 2) {
        mv.pos1++;
        mv.pos2 = mv.pos1 + 1;
    } else if (mv.route < rp.size() - 1) {
        mv.route++;
        mv.pos1 = 0;
        mv.pos2 = 1;
    } else {
        return false;
    }
    return true;
}

void
IntraSwapNeighborhoodExplorer::MakeMove(RoutePlan &rp,
                                      const IntraSwap &mv) const {
    std::swap(rp[mv.route][mv.pos1], rp[mv.route][mv.pos2]);
    // update timetable
    UpdateRouteTimetable(rp.timetable(mv.route), rp[mv.route]);
}

int
IntraSwapNeighborhoodExplorer::DeltaCostFunction(const RoutePlan &rp,
                                               const IntraSwap &mv) const {
    // ugly reduntant data
    routes_.clear();
    routes_.push_back(rp[mv.route]);
    std::swap(routes_[0][mv.pos1], routes_[0][mv.pos2]);
    timetable_.clear();
    timetable_.resize(1);
    if (!routes_[0].IsExcList())
        UpdateRouteTimetable(timetable_[0], routes_[0]);

    delta_num_order_late_return = 0;
    return DeltaObjective(rp, mv) + DeltaViolations(rp, mv);
}

int
IntraSwapNeighborhoodExplorer::DeltaObjective(const RoutePlan &rp,
                                            const IntraSwap &mv) const {
    #ifdef _NE_DEBUG_H_
    int time_cost = DeltaTimeViolationCost(rp, mv, 10);
    int trans_cost = DeltaTranportationCost(rp, mv);
    std::cout << "Delta obj: " << time_cost << ", " << trans_cost << std::endl;
    return time_cost + trans_cost;
    #else
    return DeltaTimeViolationCost(rp, mv, 10) + DeltaTranportationCost(rp, mv);
    #endif
}

int
IntraSwapNeighborhoodExplorer::DeltaViolations(const RoutePlan &rp,
                                             const IntraSwap &mv) const {
    int w = 1;
    #ifdef _NE_DEBUG_H_
    int cap_cost = DeltaCapExceededCost(rp, mv, w);
    int late_cost = DeltaLateReturnCost(rp, mv, w);
    std::cout << "Delta vio: " << cap_cost << ", " << late_cost << std::endl;
    return cap_cost + late_cost;
    #else
    return DeltaCapExceededCost(rp, mv, w) + DeltaLateReturnCost(rp, mv, this->vio_wt);
    #endif
}


int
IntraSwapNeighborhoodExplorer::DeltaTimeViolationCost(const RoutePlan &rp,
                                        const IntraSwap &mv, int weight) const {
    int delta = 0;
    if (!rp[mv.route].IsExcList()) {
        int late_ret_bef = 0, late_ret_aft = 0;
        int cost_bef = RouteCostsOnTimeWindow(rp[mv.route],
                                              rp.timetable(mv.route),
                                              &late_ret_bef);
        int cost_aft = RouteCostsOnTimeWindow(routes_[0],
                                              timetable_[0],
                                              &late_ret_aft);
        delta += cost_aft - cost_bef;
        delta_num_order_late_return += late_ret_aft - late_ret_bef;
        // unsigned pos = mv.pos1;
        // if (mv.pos1 > mv.pos2)
        //     pos = mv.pos2;
        // delta += DeltaRouteTimeViolation(rp, 0, mv.route, pos);
    }

    return (weight * delta);
}

int
IntraSwapNeighborhoodExplorer::DeltaTranportationCost(const RoutePlan &rp,
                                                    const IntraSwap &mv) const {
    int delta = 0;
    if (!rp[mv.route].IsExcList()) {
        const Billing *cr = in.FindBilling(rp[mv.route].get_vehicle());
        delta += cr->GetCostComponent().Cost(routes_[0]) -
                 cr->GetCostComponent().Cost(rp[mv.route]);
    }
    return delta;
}

int
IntraSwapNeighborhoodExplorer::DeltaCapExceededCost(const RoutePlan &rp,
                                        const IntraSwap &mv, int weight) const {
    return 0;
}

int
IntraSwapNeighborhoodExplorer::DeltaLateReturnCost(const RoutePlan &rp,
                                        const IntraSwap &mv, int weight) const {
    int delta = delta_num_order_late_return;
    // int shutdown_time = in.get_return_time() + 3600;  // plus 1 hour

    // if (rp[mv.route].IsExcList())
    //     return 0;

    // for (unsigned j = mv.pos1; j < rp[mv.route].size(); ++j) {
    //     if (rp(mv.route, j) > shutdown_time)
    //         --delta;
    //     if (timetable_[0][j] > shutdown_time)
    //         ++delta;
    // }

    return (weight * delta);
}
