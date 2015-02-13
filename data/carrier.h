#ifndef _CARRIER_H
#define _CARRIER_H
#include <iostream>
#include <vector>
#include <string>
#include <utility>

class Carrier {
    // friend std::istream& operator>>(std::istream&, const Carrier&);
 public:
    Carrier(std::string cid, std::string bid):
        id(cid), id_billing(bid) { }
    std::string get_id() const { return id; }
    std::string get_billing() const { return id_billing; }
 private:
    std::string id, id_billing;
    // std::vector<bool> incompat_regions;
};
#endif
