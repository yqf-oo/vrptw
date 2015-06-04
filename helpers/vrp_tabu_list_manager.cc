#include "helpers/vrp_tabu_list_manager.h"

bool InsMoveTabuListManager::Inverse(const InsMove &mt,
                                     const InsMove &me) const {
    switch(index) {
        case 6:
            // PR6
            if (me.new_route == mt.old_route)
                return true;
        case 5:
            // PR5
            if (me.old_route == mt.new_route)
                return true;
        case 4:
            // PR4 -- involving the same order
            if (me.order == mt.order)
                return true;
        case 3:
            // PR3
            if (me.order == mt.order && me.new_route == mt.old_route)
                return true;
        case 2:
            // PR2
            if (me.new_route == mt.old_route && me.old_route == mt.new_route)
                return true;
        case 1:
            // PR1
            if (me.order == mt.order && me.old_route == mt.new_route)
                return true;
        default:
            return false;
    }
}

bool InterSwapTabuListManager::Inverse(const InterSwap &mt,
                                       const InterSwap &me) const {
    if (me.ord1 == mt.ord1 || me.ord2 == mt.ord2 ||
        me.ord1 == mt.ord2 || me.ord2 == mt.ord1)
        return true;
    return false;
}

bool IntraSwapTabuListManager::Inverse(const IntraSwap &mt,
                                       const IntraSwap &me) const {
    if (me.ord1 == mt.ord1 || me.ord2 == mt.ord2 ||
        me.ord1 == mt.ord2 || me.ord2 == mt.ord1)
        return true;
    return false;
}
