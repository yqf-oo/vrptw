#ifndef _PROB_INPUT_H_
#define _PROB_INPUT_H_
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <cassert>
#include "data/carrier.h"
#include "data/vehicle.h"
#include "data/client.h"
#include "data/order.h"
#include "data/billing.h"


// Deal with problem input
class ProbInput {
 public:
    // ignore VRPPC temporarily
    ProbInput(std::istream&);
    ~ProbInput();
    int IndexRegion(const std::string&) const;
    int FindCarrier(const std::string&) const;
    int IndexClient(const std::string&) const;
    const Client& FindClient(const std::string&) const;
    const Billing* FindBilling(int v) const;
    std::string get_depot() const { return depot_id; }
    int get_num_client() const { return num_client; }
    int get_num_order() const { return num_order; }
    int get_num_og() const { return ordergroup_vec.size(); }
    int get_num_vehicle() const { return num_vehicle; }
    int get_num_region() const { return num_region; }
    int get_num_carrier() const { return num_carrier; }
    int get_num_billing() const { return num_billing; }
    const Order& OrderVect(int i) const {
        assert(i < num_order && i >= 0);
        return order_vec[i];
    }
    const Vehicle& VehicleVect(int i) const {
        assert(i < num_vehicle && i >= 0);
        return vehicle_vec[i];
    }
    int get_dayspan() const {
        return plan_horizon.second - plan_horizon.first + 1;
    }
    int get_depart_time() const {
        return FindClient(depot_id).get_ready_time();
    }
    int get_return_time() const {
        return FindClient(depot_id).get_due_time();
    }
    const std::pair<int, int>& get_plan_horizon() const { return plan_horizon; }
    int get_distance(const std::string&, const std::string&) const;
    int get_time_dist(const std::string&, const std::string&) const;
    bool IsReachable(const Vehicle&, const Order&) const;

 private:
    void ReadDataSection(std::istream&);
    void CreateBillingStategy(std::istream&);
    void GroupOrder();
    int get_maxcap_for_order(const Order&) const;
    std::string name, depot_id;
    int num_client;
    int num_vehicle;
    int num_order;
    int num_region;
    int num_carrier;
    int num_billing;
    std::pair<int, int> plan_horizon;

    // Data section
    std::vector<Carrier> carrier_vec;
    std::vector<Vehicle> vehicle_vec;
    std::vector<Client> client_vec;
    std::vector<Order> order_vec;
    std::vector<OrderGroup> ordergroup_vec;
    // std::vector<Billing*> billing_vec;

    std::vector<std::vector<int> > distance;  // in meters
    std::vector<std::vector<int> > time_dist;  // in seconds

    // Id maps
    std::map<std::string, int> region_imap;
    std::map<std::string, int> client_imap;
    std::map<std::string, int> carrier_imap;
    std::map<std::string, Billing*> billing_imap;

    std::vector<std::vector<bool> > site_map;
};
#endif
