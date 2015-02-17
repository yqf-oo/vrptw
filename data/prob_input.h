#ifndef _PROB_INPUT_H_
#define _PROB_INPUT_H_
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include "data/carrier.h"
#include "data/vehicle.h"
#include "data/client.h"
#include "data/order.h"


// Deal with problem input
class ProbInput{
 public:
    // ignore VRPPC temporarily
    ProbInput(std::fstream&);
    ~ProbInput();
    const Client& FindClient(const string&) const;
    const Carrier& FindCarrier(const string &carrier_id) const {
        return carrier_imap[carrier_id];
    }
    const Billing* FindBillingComponent(const string &billing_id) const {
        return billing_imap[billing_id];
    }
    unsigned get_num_client() const { return num_client; }
    unsigned get_num_order() const { return num_order; }
    unsigned get_num_og() const { return ordergroup_vec.size(); }
    unsigned get_num_vehicle() const { return num_vehicle; }
    unsigned get_num_region() const { return num_region; }
    unsigned get_num_carrier() const { return num_carrier; }
    unsigned get_num_billing() const { return num_billing; }
    const Order& OrderVect(unsigned i) {
        assert(i < num_order);
        return order_vec(i);
    }
    const Vehicle& VehicleVect(unsigned i) {
        assert(i < num_vehicle);
        return vehicle_vec(i);
    }
    int get_dayspan() const { 
        return plan_horizon.second - plan_horizon.fisrt + 1;
    }
    int get_depart_time() const { 
        return client_vec[client_imap[depot_id]].get_ready_time();
    }
    const std::pair<int, int>& get_plan_horizon() const { return plan_horizon; }
    int get_distance(const string&, const string&) const;
    int get_time_dist(const string&, const string&) const;
    bool IsReachable(const Vehicle&, const Order&);

 private:
    void ReadDataSection(std::fstream&);
    void GroupOrder();
    unsigned get_maxcap_for_order(const Order&);
    string name, depot_id;
    unsigned num_client;
    unsigned num_vehicle;
    unsigned num_order;
    unsigned num_region;
    unsigned num_carrier;
    unsigned num_billing;
    std::pair<int, int> plan_horizon;

    // Data section
    std::vector<Carrier> carrier_vec;
    std::vector<Vehicle> vehicle_vec;
    std::vector<Client> client_vec;
    std::vector<Order> order_vec;
    std::vector<OrderGroup> ordergroup_vec;
    // std::vector<Billing*> billing_vec;

    std::vector<std::vector<int>> distace;  // in meters
    std::vector<std::vector<int>> time_dist;  // in seconds

    // Id maps
    std::map<std::string, int> region_imap;
    std::map<std::string, int> client_imap;
    std::map<std::string, int> carrier_imap;
    std::map<std::string, Billing*> billing_imap;

    std::vector<std::vector<bool>> site_map;
};
#endif
