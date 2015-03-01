#include <helpers/NeighborhoodExplorer.hh>
#include <vector>
#include <string>
#include "data/move.h"
#include "data/route.h"
#include "data/prob_input.h"
#include "helpers/vrp_state_manager.h"

typedef NeighborhoodExplorer<ProbInput, RoutePlan, InsMove> NeiExp;

template <class Move>
class TabuNeighborhoodExplorer:
public NeighborhoodExplorer<ProbInput, RoutePlan, Move> {
 public:
    virtual void RandomMove(const RoutePlan&, Move&) const = 0;
    virtual void FirstMove(const RoutePlan&, Move&) const = 0;
    virtual bool NextMove(const RoutePlan&, Move&) const = 0;
    virtual void MakeMove(RoutePlan&, const Move&) const = 0;
 protected:
    TabuNeighborhoodExplorer(const ProbInput& in,
                             VRPStateManager& sm, string nm):
        NeighborhoodExplorer<ProbInput, RoutePlan, Move>(in, sm, nm) { }
    const std::vector<int> UpdateRouteTimetable(std::vector<int>&, const Route&);
    int ComputeRouteTimeViolation(const RoutePlan&,
                                  int, unsigned, unsigned);
    std::vector<Route> routes;
    std::vector<std::vector<int>> timetable;
};

class InsMoveNeighborhoodExplorer: public TabuNeighborhoodExplorer<InsMove> {
 public:
    InsMoveNeighborhoodExplorer(const ProbInput &in, VRPStateManager &sm):
        TabuNeighborhoodExplorer<InsMove>(in, sm, "InsMoveNeighborhood") { }

    // move generation
    void RandomMove(const RoutePlan&, InsMove&) const;
    void FirstMove(const RoutePlan&, InsMove&) const;
    bool NextMove(const RoutePlan&, InsMove&) const;
    bool FeasibleMove(const RoutePlan&, const InsMove&) const;
    void MakeMove(RoutePlan&, const InsMove&) const;

    // delta cost
    int DeltaCostFunction(const RoutePlan&, const InsMove&) const;
    int DeltaObjective(const RoutePlan&, const InsMove &) const;
    int DeltaViolations(const RoutePlan&, const InsMove &) const;

 private:
    void AnyRandomMove(const RoutePlan&, InsMove&) const;
    bool AnyNextMove(const RoutePlan&, InsMove&) const;
    int DeltaDateViolationCost(const RoutePlan&,
                                    const InsMove&, int) const;     // s1
    int DeltaTimeViolationCost(const RoutePlan&,
                                    const InsMove&, int) const;     // s2
    int DeltaOptOrderCost(const RoutePlan&,
                               const InsMove&, int) const;          // s3
    int DeltaTranportationCost(const RoutePlan&, const InsMove&) const;
                                                                    // s4
    int DeltaCapExceededCost(const RoutePlan&,
                                  const InsMove&, int) const;       // h1
    int DeltaLateReturnCost(const RoutePlan&, const InsMove&, int) const;
                                                                    // h2
    // RoutePlan old_state;
};

class InterSwapNeighborhoodExplorer:
public TabuNeighborhoodExplorer<InterSwap> {
 public:
    InterSwapNeighborhoodExplorer(const ProbInput &in, VRPStateManager &sm):
        TabuNeighborhoodExplorer<InterSwap>(in, sm, "InterSwapNeighborhood") { }

    // move generation
    void RandomMove(const RoutePlan&, InterSwap&) const;
    void FirstMove(const RoutePlan&, InterSwap&) const;
    bool NextMove(const RoutePlan&, InterSwap&) const;
    bool FeasibleMove(const RoutePlan&, const InterSwap&) const;
    void MakeMove(RoutePlan&, const InterSwap&) const;

    // delta cost
    int DeltaCostFunction(const RoutePlan&, const InterSwap&) const;
    int DeltaObjective(const RoutePlan&, const InterSwap &) const;
    int DeltaViolations(const RoutePlan&, const InterSwap &) const;

 private:
    void AnyRandomMove(const RoutePlan&, InterSwap&) const;
    bool AnyNextMove(const RoutePlan&, InterSwap&) const;
    int DeltaDateViolationCost(const RoutePlan&,
                                    const InterSwap&, int) const;   // s1
    int DeltaTimeViolationCost(const RoutePlan&,
                                    const InterSwap& int) const;    // s2
    int DeltaOptOrderCost(const RoutePlan&,
                                    const InterSwap&, int) const;   // s3
    int DeltaTranportationCost(const RoutePlan&, const InterSwap&) const;
                                                                    // s4
    int DeltaCapExceededCost(const RoutePlan&,
                                  const InterSwap&, int) const;     // h1
    int DeltaLateReturnCost(const RoutePlan&, const InterSwap&, int) const;
                                                                    // h2
};

class IntraSwapNeighborhoodExplorer:
public TabuNeighborhoodExplorer<IntraSwap> {
 public:
    IntraSwapNeighborhoodExplorer(const ProbInput &in, VRPStateManager &sm):
        TabuNeighborhoodExplorer<IntraSwap>(in, sm, "IntraSwapNeighborhood") { }

    // move generation
    void RandomMove(const RoutePlan&, IntraSwap&) const;
    void FirstMove(const RoutePlan&, IntraSwap&) const;
    bool NextMove(const RoutePlan&, IntraSwap&) const;
    bool FeasibleMove(const RoutePlan&, const InsMove&) const;
    void MakeMove(RoutePlan&, const IntraSwap&) const;

    // delta cost
    int DeltaCostFunction(const RoutePlan&, const IntraSwap&) const;
    int DeltaObjective(const RoutePlan&, const IntraSwap &) const;
    int DeltaViolations(const RoutePlan&, const IntraSwap &) const;

 private:
    void AnyRandomMove(const RoutePlan&, IntraSwap&) const;
    bool AnyNextMove(const RoutePlan&, IntraSwap&) const;
    int DeltaDateViolationCost(const RoutePlan&,
                                    const IntraSwap&, int) const;    // s1
    int DeltaTimeViolationCost(const RoutePlan&,
                                    const IntraSwap&, int) const;    // s2
    int DeltaOptOrderCost(const RoutePlan&,
                               const IntraSwap&, int) const;         // s3
    int DeltaTranportationCost(const RoutePlan&, const IntraSwap&) const;
                                                                     // s4
    int DeltaCapExceededCost(const RoutePlan&,
                                  const IntraSwap&, int) const;      // h1
    int DeltaLateReturnCost(const RoutePlan&, const IntraSwap&, int) const;
                                                                     // h2
};

// Implementation

template <class Move>
void
TabuNeighborhoodExplorer<Move>::UpdateRouteTimetable(std::vector<int> &ret, const Route &r) {
    int arrive_time = in.get_depart_time();
    int stop_time = in.get_depart_time();
    unsigned route_size = r.size();
    for (int i = 0; i <= route_size; ++i) {
        std::string client_to(in.get_depot());
        if (i < route_size)
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
        ret.push_back(arrive_time);
    }
    return ret;
}

template <class Move> int
TabuNeighborhoodExplorer<Move>::ComputeRouteTimeViolation(const RoutePlan &rp,
                                    int isnew, unsigned route, unsigned pos) {
    int delta = 0;
    for (int i = pos; i < rp[route].size(); ++i) {
        client = in.OrderVect(rp[route][i]).get_client();
        int duetime = in.FindClient(client).get_due_time();
        int arrive_time = sm(route, i);
        if (arrive_time > duetime)
            delta -= arrive_time - duetime;
    }
        // after move
    for (int i = pos; i < routes[isnew].size(); ++i) {
        client = in.OrderVect(routes[isnew][i]).get_client();
        int duetime = in.FindClient(client).get_due_time();
        if (timetable[isnew][i] > duetime)
            delta += timetable[isnew][i] - duetime;
    }
    return delta;
}
