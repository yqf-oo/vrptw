#ifndef _NEIGHBOR_H_
#define _NEIGHBOR_H_

class InsMove {
 public:
    // InsMove(unsigned o, unsigned ro, unsigned po,
    //         unsigned rn, unsigned pn):
    //     order(o), old_route(ro), old_pos(po),
    //     new_route(rn), new_pos(pn) { }

    bool operator==(const InsMove&) const;
    bool operator!=(const InsMove&) const;
    unsigned order;
    unsigned old_route, old_pos;
    unsigned new_route, new_pos;
};

class InterSwap {
 public:
    // InterSwap(unsigned o1, unsigned o2, unsigned r1,
    //           unsigned r2, unsigned p1, unsigned p2):
    //     ord1(o1), ord2(o2), route1(r1),
    //     route2(r2), pos1(p1), pos2(p2) { }

    bool operator==(const InterSwap&) const;
    bool operator!=(const InterSwap&) const;
    unsigned ord1, ord2;
    unsigned route1, route2;
    unsigned pos1, pos2;
};

class IntraSwap {
 public:
    // IntraSwap(unsigned o1, unsigned o2, unsigned r,
    //           unsigned p1, unsigned p2):
    //     ord1(o1), ord2(o2), route(r), pos1(p1), pos2(p2) { }

    bool operator==(const IntraSwap&) const;
    bool operator!=(const IntraSwap&) const;
    unsigned ord1, ord2, route;
    unsigned pos1, pos2;
};

#endif
