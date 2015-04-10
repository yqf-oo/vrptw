#include "solvers/vrp_token_ring_observer.h"
#include "solvers/vrp_token_ring_search.h"

void TokenRingObserver::NotifyRestart(TokenRingSearch &s, unsigned restart) {
    log << "Restart " << restart << " trials " << std::endl;
}

void TokenRingObserver::NotifyRound(TokenRingSearch &s) {
    log << "Round " << s.round_ << "/" << s.max_rounds_ << " finished (idle "
        << s.idle_rounds_ << "/" << s.max_idle_rounds_ << ")" << std::endl;
}

void TokenRingObserver::NotifyRunnerStart(TokenRingSearch &s) {
    log << "Starting runner " << s.current_runner_ << " of solver " << s.name
        << std::endl;
}

void TokenRingObserver::NotifyRunnerStop(TokenRingSearch &s) {
    log << "Runner: " << s.current_runner_ << ", cost: "
        << s.p_runners[s.current_runner_]->GetStateCost()
        // << ", distance from starting/best states "
        // << s.sm.StateDistance(s.current_state, s.p_runners_[s.current_runner]->GetState())
        // << "/" << s.sm.StateDistance(s.best_state, s.runners[s.current_runner]->GetState())
        << " (" << s.p_runners[s.current_runner_]->GetIterationsPerformed()
        << " iterations, time " << s.chrono.TotalTime() << "), Rounds "
        << s.round_ << "/" << s.max_rounds_ << ", Idle rounds "
        << s.idle_rounds_ << "/" << s.max_idle_rounds_ << std::endl;
}
