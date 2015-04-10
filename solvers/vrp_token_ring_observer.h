#ifndef _VRP_TOKEN_RING_OBSERVER_H_
#define _VRP_TOKEN_RING_OBSERVER_H_
#include <iostream>

class TokenRingSearch;
class TokenRingObserver {
 public:
  TokenRingObserver(std::ostream &log_os = std::cout): log(log_os) { }
  void NotifyRestart(TokenRingSearch&, unsigned);
  void NotifyRound(TokenRingSearch&);
  void NotifyRunnerStart(TokenRingSearch&);
  void NotifyRunnerStop(TokenRingSearch&);
 private:
  std::ostream &log;
};

#endif
