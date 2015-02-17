#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include "data/prob_input.h"

const int kBufSize = 200;

ProbInput::ProbInput(std::fstream &input) {
    std::string tmp;
    input >> tmp >> tmp;
    name = tmp;  // case name

    getline(input, tmp);
    getline(input, tmp);
    getline(input, tmp);

    char buffer[kBufSize];
    input.getline(buffer, kBufSize, ':');
    input >> num_client;
    getline(input, tmp);

    input.getline(buffer, kBufSize, ':');
    input >> num_order;
    getline(input, tmp);

    input.getline(buffer, kBufSize, ':');
    input >> num_vehicle;
    getline(input, tmp);

    input.getline(buffer, kBufSize, ':');
    input >> num_carrier;
    getline(input, tmp);

    input.getline(buffer, kBufSize, ':');
    input >> num_billing;
    getline(input, tmp);

    input.getline(buffer, kBufSize, ':');
    input >> num_region;
    getline(input, tmp);

    getline(input, tmp);  // EDGE_WEIGHT_TYPE
    getline(input, tmp);  // EDGE_WEIGHT_FORMAT

    input.getline(buffer, kBufSize, ':');
    input >> plan_horizon.first >> tmp >> plan_horizon.second;
    getline(input, tmp);

    ReadDataSection(input);
}

ProbInput::~ProbInput() {

}

void ProbInput::ReadDataSection(std::fstream &input) {
    std::string tmp;
    input >> tmp;
    input >> tmp >> tmp;
    input >> depot_id;
    input >> tmp >> tmp;
    // REGIONS
    for (int i = 0; i < num_region; ++i) {
        input >> tmp;
        region_imap[tmp] = i;
    }

    // BILLINGS

    string id1, id2;
    // CARRIERS
    input >> tmp >> tmp;
    site_map.resize(num_carrier, std::vector<bool>(num_region, true));
    for (int i = 0; i < num_carrier; ++i) {
        int num_incompatible_regions;
        input >> id1 >> id2 >> num_incompatible_regions;
        string region_id;
        for (int j = 0; j < num_incompatible_regions; ++j){
            intput >> region_id;
            int region_ind = region_imap[region_id];
            assert(region_ind < num_region);
            site_map[i][region_ind] = false;
        }
        carrier_vec.push_back(Carrier(id1, id2));
        carrier_imap[id1] = i;
    }

    // VEHICLES
    input >> tmp >> tmp;
    vehicle_vec.resize(num_vehicle);
    for (int i = 0; i < num_vehicle; ++i)
        input >> vehicle_vec[i];

    // CLIENTS
    input >> tmp >> tmp;
    client_vec.resize(num_client);
    for (int i = 0; i < num_client; ++i) {
        input >> client_vec[i];
        client_imap[client_vec[i].get_id()] = i;
    }

    // ORDERS
    input >> tmp >> tmp;
    order_vec.resize(num_order);
    for (int i = 0; i < num_order; ++i)
        input >> order_vec[i];
    GroupOrder();

    // EDGES
    input >> tmp >> tmp;
    distance.resize(num_client, std::vector<int>(num_client, -1));
    time_dist.resize(num_client, std::std::vector<int>(num_client, -1));
    while (input >> tmp) {
        if (tmp == "END")
            break;

        std::istringstream ss(tmp);
        double dist_km;
        int dist_sec, ind1, ind2;
        ss >> id1 >> id2 >> dist_km >> dist_sec;
        ind1 = client_imap[id1];
        ind2 = client_imap[id2];

        assert(ind1 < num_client && ind2 < num_client);
        distance[ind1][ind2] = dist_km * 1000;  // to metres
        time_dist[ind1][ind2] = dist_sec;
    }
}

void ProbInput::GroupOrder() {
    OrderGroup og(order_vec[0]);
    ordergroup_vec.push_back(og);
    for (int i = 1; i < num_order; i++){
        int og_size = ordergroup_vec.size();
        for (int j = 0; j < og_size; ++j) {
            if (ordergroup_vec[j].IsGroupCompatible(order_vec[i])) {
                int group_qty = order_vec[i].qty;
                unsigned max_cap = get_maxcap_for_order(order_vec[i]);
                if (group_qty + ordergroup_vec[j].get_cap() < max_cap) {
                    ordergroup_vec[j].insert(order_vec[i]);
                    order_vec[i].set_group(j);
                } else {
                    ordergroup_vec.push_back(order_vec[i]);
                    order_vec[i].set_group(order_vec.size() - 1);
                }
            }
        }
    }
}

unsigned ProbInput::get_maxcap_for_order(const Order &o) {
    unsigned max = 0;
    for (int k = 0; k < num_vehicle; ++k) {
        if (IsReachable(vehicle_vec[k], order_vec[i]))
            if (max < vehicle_vec[k].get_cap())
                max = vehicle_vec[k].get_cap();
        }
    }
    return max;
}

int ProbInput::get_distance(const std::string &cli_from,
                             const std::string &cli_to) {
    int id_from = client_imap[cli_from];
    int id_to = client_imap[cli_to];
    assert(id_from < num_client && id_to < num_client);
    return distance[id_from][id_to];
}

int ProbInput::get_time_dist(const std::string &cli_from,
                              const std::string &clit_to) {
    int id_from = client_imap[cli_from];
    int id_to = client_imap[cli_to];
    assert(id_from < num_client && id_to < num_client);
    return time_dist[id_from][id_to];
}

Client& ProbInput::FindClient(const string &client_id) {
    int c_index = client_imap[client_id];
    assert(c_index < num_client);
    return client_vec[c_index];
}

bool ProbInput::IsReachable(const Vehicle &v, const Order &o) {
    int cr_index = carrier_imap[v.get_carrier()];
    int r_index = region_imap[FindClient(o.get_client()).get_region()];
    assert(cr_index < num_carrier && r_index < num_region);
    return site_map[cr_index][r_index];
}
