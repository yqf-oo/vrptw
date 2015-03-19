#include "helpers/billing_cost_component.h"
#include <cmath>
#include <utility>
#include "data/route.h"
#include "data/billing.h"
#include "data/prob_input.h"

std::pair<unsigned, unsigned>
BillingCostComponent::MaxRateLoad(const Route &r) {
    int vehicle = r.get_vehicle();
    unsigned max_rate = 0, load = 0;
    const Billing *cr = in.FindBilling();
    for (int i = 0; i < r.size(); ++i) {
        string client_id = in.OrderVect(r[i]).get_client();
        int rindex = in.IndexRegion(in.FindClient(client_id).get_region());
        if (cr->get_load_cost(rindex) > max_rate)
            max_rate = cr->get_load_cost(rindex);
        load += in.OrderVect(r[i]).get_demand();
    }
    return std::pair<unsigned, unsigned>(max_rate, load);
}

// bt1
int DistanceBillingCostComponent::ComputeCost(const Route &r) const {
    int vehicle = r.get_vehicle();
    return (r.length() * in.FindBilling(vehicle)->get_km_rate());
}

// bt2
int DistanceLoadBillingCostComponent::ComputeCost(const Route &r) const {
    if (IsFull(r)) {
        std::pair<unsigned, unsigned> ml = MaxRateLoad(r);
        unsigned max_rate = ml.first, load = ml.second;
        // Notice that the load is in kg not in g!
        cost = load * max_rate;
    } else {
        const Billing* cr = in.FindBilling(r.get_vehicle());
        cost = r.length() * cr->get_km_rate();
    }
    return cost;
}

bool DistanceLoadBillingCostComponent::IsFull(const Route &r) const {
    unsigned load = r.demand(in);
    int vehicle = r.get_vehicle();
    const Billing *cr = in.FindBilling(vehicle);
    double full = cr->get_full_load() / 100;
    if (load >= full * in.VehicleVect(vehicle).get_cap())
        return true;
    return false;
}

// bt3
int LoadRangeBillingCostComponent::ComputeCost(const Route &r) const {
    int vehicle = r.get_vehicle();
    const Billing *cr = in.FindBilling(vehicle);

    unsigned load =  r.demand(in);
    unsigned range = FindRange(r, load);
    unsigned max_rate =  MaxRate(r, range);


    if (range > 0) {
        // to be sure that the cost function is monotonous descreasing
        unsigned threshold_cost =  MaxRate(r, range-1) * cr->levels(range-1);

        unsigned threshold_qty = static_cast<unsigned>
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
LoadRangeBillingCostComponent::FindRange(const Route &r, unsigned load) const {
    in vehicle = r.get_vehicle();
    const Billing *cr = in.FindBilling(vehicle);

    unsigned range = 0, num_level = levels.size();
    if (load > cr->levels[num_level - 1]) {
        range = num_level;
    } else {
        while (range < num_level-1 && load > cr->levels[range])
            ++range;
    }
    return range;
}

unsigned
LoadRangeBillingCostComponent::MaxRate(const Route &r, unsigned range) const {
    int vehicle = r.get_vehicle();
    const Billing *cr = in.FindBilling(vehicle);

    unsigned max_rate = 0;
    for (int i = 0; i < r.size(); ++i) {
        string client_id = in.OrderVect(r[i]).get_client();
        int rindex = in.IndexRegion(in.FindClient(client_id).get_region());
        if (max_rate < cr->get_load_cost(rindex, range))
            max_rate = cr->get_load_cost(rindex, range);
    }
    return max_rate;
}

// bt4
int LoadFarestClientCostComponent::ComputeCost(const Route &r) const {
    int vehicle = r.get_vehicle();
    std::pair<unsigned, unsigned> ml = MaxRateLoad(r);
    unsigned  max_rate = ml.first, load = ml.second;
    return (load * max_rate);
}

// bt5
int LoadClientDependentCostComponent::ComputeCost(const Route &r) const {
    int vehicle = r.get_vehicle();
    const Billing *cr = in.FindBilling(vehicle);

    for (int i = 0; i < r.Size(); ++i) {
        string client_id = in.OrderVect(r[i]).get_client();
        int rindex = in.IndexRegion(in.FindClient(client_id).get_region());
        unsigned rate = cr->get_load_cost(rindex);
        cost += in.OrderVect(r[i]).get_demand() * rate;
    }

    return cost;
}
