#include <iostream>
#include <utility>
#include "data/client.h"

std::istream& operator>>(std::istream &is, Client &c) {
    is >> c.id >> c.id_region >> c.time_window.first
       >> c.time_window.second >> c.serv_time;
    return is;
}
