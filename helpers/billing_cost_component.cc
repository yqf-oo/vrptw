#include "helpers/billing_cost_component.h"
#include <cmath>
#include <utility>
#include "data/route.h"
#include "data/billing.h"
#include "data/prob_input.h"


// rate/load type is set to int
// bt1
int DistanceBillingCostComponent::ComputeCost(const Route &r) const {
    int vehicle = r.get_vehicle();
    const KmBilling* cr = static_cast<const KmBilling*>(in.FindBilling(vehicle));
    return (r.length(in) * cr->get_km_rate());
}

// bt2
int DistanceLoadBillingCostComponent::ComputeCost(const Route &r) const {
    int cost = 0;
    if (IsFull(r)) {
        std::pair<int, int> ml = MaxRateLoad(r);
        int max_rate = ml.first, load = ml.second;
        // Notice that the load is in kg not in g!
        cost = load * max_rate;
    } else {
        int v = r.get_vehicle();
        const LoadKmBilling* cr =
                static_cast<const LoadKmBilling*>(in.FindBilling(v));
        cost = r.length(in) * cr->get_km_rate();
    }
    return cost;
}

std::pair<int, int>
DistanceLoadBillingCostComponent::MaxRateLoad(const Route &r) const {
    int vehicle = r.get_vehicle();
    int max_rate = 0, load = 0;
    const LoadKmBilling *cr =
            static_cast<const LoadKmBilling*>(in.FindBilling(vehicle));
    for (unsigned i = 0; i < r.size(); ++i) {
        std::string client_id = in.OrderVect(r[i]).get_client();
        int rindex = in.IndexRegion(in.FindClient(client_id).get_region());
        if (cr->get_load_cost(rindex) > max_rate)
            max_rate = cr->get_load_cost(rindex);
        load += in.OrderVect(r[i]).get_demand();
    }
    return std::pair<int, int>(max_rate, load);
}

bool DistanceLoadBillingCostComponent::IsFull(const Route &r) const {
    int load = r.demand(in);
    int vehicle = r.get_vehicle();
    const LoadKmBilling *cr =
            static_cast<const LoadKmBilling*>(in.FindBilling(vehicle));
    double full = cr->get_full_load() / 100;
    if (load >= full * in.VehicleVect(vehicle).get_cap())
        return true;
    return false;
}

// bt3
int LoadRangeBillingCostComponent::ComputeCost(const Route &r) const {
    int vehicle = r.get_vehicle(), cost = 0;
    const VarLoadBilling *cr =
            static_cast<const VarLoadBilling*>(in.FindBilling(vehicle));

    int load =  r.demand(in);
    unsigned range = FindRange(r, load);
    int max_rate =  MaxRate(r, range);


    if (range > 0) {
        // to be sure that the cost function is monotonous descreasing
        int threshold_cost =  MaxRate(r, range-1) * cr->get_level(range-1);

        int threshold_qty = static_cast<int>
                    (floor(static_cast<double>(threshold_cost/max_rate)));
        if (load <= threshold_qty)
            cost = threshold_cost;
        else
            cost = load * max_rate;
    } else {
        cost = load * max_rate;
    }

    return cost;
}

unsigned
LoadRangeBillingCostComponent::FindRange(const Route &r, int load) const {
    int vehicle = r.get_vehicle();
    const VarLoadBilling *cr =
        static_cast<const VarLoadBilling*>(in.FindBilling(vehicle));

    unsigned range = 0, num_level = cr->get_num_range();
    if (load > cr->get_level(num_level - 1)) {
        range = num_level;
    } else {
        while (range < num_level-1 && load > cr->get_level(range))
            ++range;
    }
    return range;
}

int
LoadRangeBillingCostComponent::MaxRate(const Route &r, unsigned range) const {
    int vehicle = r.get_vehicle();
    const VarLoadBilling *cr =
            static_cast<const VarLoadBilling*>(in.FindBilling(vehicle));

    int max_rate = 0;
    for (unsigned i = 0; i < r.size(); ++i) {
        std::string client_id = in.OrderVect(r[i]).get_client();
        int rindex = in.IndexRegion(in.FindClient(client_id).get_region());
        if (max_rate < cr->get_load_cost(rindex, range))
            max_rate = cr->get_load_cost(rindex, range);
    }
    return max_rate;
}

// bt4
int LoadFarestClientCostComponent::ComputeCost(const Route &r) const {
    // int vehicle = r.get_vehicle();
    std::pair<unsigned, unsigned> ml = MaxRateLoad(r);
    int  max_rate = ml.first, load = ml.second;
    return (load * max_rate);
}

std::pair<int, int>
LoadFarestClientCostComponent::MaxRateLoad(const Route &r) const {
    int vehicle = r.get_vehicle();
    int max_rate = 0, load = 0;
    const LoadBilling *cr =
            static_cast<const LoadBilling*>(in.FindBilling(vehicle));
    for (unsigned i = 0; i < r.size(); ++i) {
        std::string client_id = in.OrderVect(r[i]).get_client();
        int rindex = in.IndexRegion(in.FindClient(client_id).get_region());
        if (cr->get_load_cost(rindex) > max_rate)
            max_rate = cr->get_load_cost(rindex);
        load += in.OrderVect(r[i]).get_demand();
    }
    return std::pair<int, int>(max_rate, load);
}

// bt5
int LoadClientDependentCostComponent::ComputeCost(const Route &r) const {
    int vehicle = r.get_vehicle(), cost = 0;
    const LoadBilling *cr =
            static_cast<const LoadBilling*>(in.FindBilling(vehicle));

    for (unsigned i = 0; i < r.size(); ++i) {
        std::string client_id = in.OrderVect(r[i]).get_client();
        int rindex = in.IndexRegion(in.FindClient(client_id).get_region());
        int rate = cr->get_load_cost(rindex);
        cost += in.OrderVect(r[i]).get_demand() * rate;
    }

    return cost;
}
