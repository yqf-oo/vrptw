#include "data/move.h"

bool InsMove::operator==(const InsMove &m) const {
    return (order == m.order) && (old_roue == m.old_route)
           && (old_pos == m.old_pos) && (new_route == m.new_route)
           && (new_pos == m.new_pos);
}

bool InsMove::operator!=(const InsMove &m) const {
    return !(operator==(m));
}

bool InterSwap::operator==(const InterSwap &m) const {
    return (ord1 == m.ord1) && (ord2 == m.ord2)
           && (route1 == m.route1) && (route2 == m.route2)
           && (pos1 == m.pos1) && (pos2 == m.pos2);
}

bool InterSwap::operator!=(const InterSwap &m) const {
    return !(operator==(m));
}

bool IntraSwap::operator==(const IntraSwap &m) const {
    return (ord1== m.ord1) && (ord2 == m.ord2)
           && (route1 == m.route1) && (pos1 == m.pos1)
           && (pos2 == m.pos2);
}

bool IntraSwap::operator==(const IntraSwap &m) const {
    return !(operator==(m));
}
