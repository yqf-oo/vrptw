#include "data/prob_input.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <cassert>
#include <cmath>
#include "helpers/billing_cost_component.h"

const int kBufSize = 200;

ProbInput::ProbInput(std::istream &input) {
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
    UpdateReachabilityMap();
    GroupOrder();
}

ProbInput::~ProbInput() {
    std::map<std::string, Billing*>::iterator map_it;
    map_it = billing_imap.begin();
    while (map_it != billing_imap.end()) {
        delete map_it->second;
        ++map_it;
    }
}

void ProbInput::ReadDataSection(std::istream &input) {
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
    CreateBillingStategy(input);

    std::string id1, id2;
    // CARRIERS
    input >> tmp >> tmp;
    site_map.resize(num_carrier, std::vector<bool>(num_region, true));
    for (int i = 0; i < num_carrier; ++i) {
        int num_incompatible_regions;
        input >> id1 >> id2 >> num_incompatible_regions;
        std::string region_id;
        for (int j = 0; j < num_incompatible_regions; ++j) {
            input >> region_id;
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


    // EDGES
    input >> tmp >> tmp;
    getline(input, tmp);
    distance.resize(num_client, std::vector<int>(num_client, -1));
    time_dist.resize(num_client, std::vector<int>(num_client, -1));
    while (getline(input, tmp)) {
        if (tmp == "END") break;
        std::istringstream ss(tmp);
        double dist_km;
        int dist_sec, ind1, ind2;
        ss >> id1 >> id2 >> dist_km >> dist_sec;
        ind1 = IndexClient(id1);
        ind2 = IndexClient(id2);

        // std::cout << id1 << ':' << ind1 << ','
        //           << id2 << ':' << ind2 << std::endl;
        assert(ind1 >= 0 && ind1 < num_client);
        assert(ind2 >= 0 && ind2 < num_client);
        distance[ind1][ind2] = static_cast<unsigned>(dist_km + 0.5);
        time_dist[ind1][ind2] = dist_sec;
    }
}

void ProbInput::CreateBillingStategy(std::istream &input) {
    int weight = 1;
    DistanceBillingCostComponent* dcc =
        new DistanceBillingCostComponent(*this, weight);
    DistanceLoadBillingCostComponent* dlcc =
        new DistanceLoadBillingCostComponent(*this, weight);
    LoadRangeBillingCostComponent* lrcc =
        new LoadRangeBillingCostComponent(*this, weight);
    LoadFarestClientCostComponent* lfcc =
        new LoadFarestClientCostComponent(*this, weight);
    LoadClientDependentCostComponent* lcc =
        new LoadClientDependentCostComponent(*this, weight);

    double n;
    std::string tmp, id, type;
    char buffer[kBufSize];
    input >> tmp;
    for (int i = 0; i < num_billing; ++i) {
        input >> id >> type;
        input.getline(buffer, kBufSize);
        if (type == "bt1") {
            input >> n;
            unsigned rate = static_cast<unsigned>(ceil(n*1000));
            KmBilling *kmc = new KmBilling(id, type, rate);
            kmc->SetCostComponent(dcc);
            billing_imap[id] = kmc;
        } else if (type == "bt2") {
            LoadKmBilling *lkc = new LoadKmBilling(id, type);
            lkc->ReadInputData(input, num_region);
            lkc->SetCostComponent(dlcc);
            billing_imap[id] = lkc;
        } else if (type == "bt3") {
            VarLoadBilling *vlc = new VarLoadBilling(id, type);
            vlc->ReadInputData(input, num_region);
            vlc->SetCostComponent(lrcc);
            billing_imap[id] = vlc;
        } else if (type == "bt4") {
            LoadBilling *lfc = new LoadBilling(id, type);
            lfc->ReadInputData(input, num_region);
            lfc->SetCostComponent(lfcc);
            billing_imap[id] = lfc;
        } else if (type == "bt5") {
            LoadBilling *ldc = new LoadBilling(id, type);
            ldc->ReadInputData(input, num_region);
            ldc->SetCostComponent(lcc);
            billing_imap[id] = ldc;
        } else {
            std::cout << "Billing type not implemented" << std::endl;
            assert(false);
        }
    }
}

void ProbInput::GroupOrder() {
    bool add = false;
    OrderGroup og(order_vec[0]);
    ordergroup_vec.push_back(og);
    for (int i = 1; i < num_order; ++i) {
        int og_size = ordergroup_vec.size();
        for (int j = 0; j < og_size; ++j) {
            if (ordergroup_vec[j].IsGroupCompatible(order_vec[i])) {
                int group_qty = ordergroup_vec[j].get_demand();
                int max_cap = get_maxcap_for_order(i);
                if (group_qty +  order_vec[i].get_demand() <= max_cap) {
                    ordergroup_vec[j].insert(order_vec[i]);
                    order_vec[i].set_group(j);
                    add = true;
                }
            }
        }
        if (!add) {
            ordergroup_vec.push_back(OrderGroup(order_vec[i]));
            order_vec[i].set_group(ordergroup_vec.size() - 1);
        }
        add = false;
    }
}

int ProbInput::get_maxcap_for_order(int o) const {
    unsigned max = 0;
    for (int k = 0; k < num_vehicle; ++k) {
        if (IsReachable(k, o)) {
            if (max < vehicle_vec[k].get_cap())
                max = vehicle_vec[k].get_cap();
        }
    }
    return max;
}

int ProbInput::get_distance(const std::string &cli_from,
                             const std::string &cli_to) const {
    int id_from = IndexClient(cli_from);
    int id_to = IndexClient(cli_to);
    assert(id_from < num_client && id_to < num_client);
    return distance[id_from][id_to];
}

int ProbInput::get_time_dist(const std::string &cli_from,
                              const std::string &cli_to) const {
    int id_from = IndexClient(cli_from);
    int id_to = IndexClient(cli_to);
    assert(id_from < num_client && id_to < num_client);
    return time_dist[id_from][id_to];
}

int ProbInput::IndexRegion(const std::string &region_id) const {
    std::map<std::string, int>::const_iterator it;
    it = region_imap.find(region_id);
    if (it != region_imap.end())
        return it->second;
    return -1;  // not found
}

int ProbInput::FindCarrier(const std::string &carrier_id) const {
    std::map<std::string, int>::const_iterator it;
    it = carrier_imap.find(carrier_id);
    if (it != carrier_imap.end())
        return it->second;
    return -1;  // not found
}

int ProbInput::IndexClient(const std::string &client_id) const {
    std::map<std::string, int>::const_iterator it;
    it = client_imap.find(client_id);
    if (it != client_imap.end())
        return it->second;
    return -1;
}

const Client& ProbInput::FindClient(const std::string &client_id) const {
    int c = IndexClient(client_id);
    return client_vec[c];
}

const Billing* ProbInput::FindBilling(int vehicle) const {
    int cr_index = FindCarrier(vehicle_vec[vehicle].get_carrier());
    std::map<std::string, Billing*>::const_iterator it;
    it = billing_imap.find(carrier_vec[cr_index].get_billing());
    if (it != billing_imap.end())
        return it->second;
    return NULL;
}

void ProbInput::UpdateReachabilityMap() {
    rmap.resize(num_vehicle, std::vector<bool>(num_order, true));
    for (int i = 0; i < num_vehicle; ++i) {
        for (int j = 0; j < num_order; ++j) {
            int cr_index = FindCarrier(vehicle_vec[i].get_carrier());
            std::string client_id = order_vec[j].get_client();
            int r_index = IndexRegion(FindClient(client_id).get_region());
            assert(cr_index < num_carrier && r_index < num_region);
            rmap[i][j] = site_map[cr_index][r_index];
        }
    }
}

bool ProbInput::IsReachable(int v, int o) const {
    assert(v < num_vehicle && o < num_order);
    return rmap[v][o];
}

bool ProbInput::IsReachable(const Vehicle &v, const Order &o) const {
    int cr_index = FindCarrier(v.get_carrier());
    int r_index = IndexRegion(FindClient(o.get_client()).get_region());
    assert(cr_index < num_carrier && r_index < num_region);
    return site_map[cr_index][r_index];
}
