#include <iostream>
#include "carrier.h"

std::istream& Crarrier::operator>>(std::istream &is, const Carrier &cr){
	int num_incompatible_regions = 0;
	is >> cr.id >> cr.id_billing >> num_incompatible_regions;
	for(int i = 0; i < num_incompatible_regions; ++i)
		is >> cr.incompat_regions;
	return is;
}
