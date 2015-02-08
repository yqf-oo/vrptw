#ifndef _PROB_INPUT_H_
#define _PROB_INPUT_H_
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

// Deal with problem input
class ProbInput{
    friend std::istream& operator>>(std::istream&, const ProbInput&);
 public:
    // ignore VRPPC temporarily
    ProbInput(ifstream&);
 private:
};
#endif
