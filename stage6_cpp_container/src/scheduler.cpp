#include "scheduler.hpp"
#include "overdue_task.hpp"
#include <algorithm>

Plan Scheduler::build(std::vector<std::unique_ptr<BaseTask>> tasks) {
    std::sort(tasks.begin(), tasks.end(),
              [](const auto& a, const auto& b) {
                  const bool aOver = dynamic_cast<const OverdueTask*>(a.get()) != nullptr;
                  const bool bOver = dynamic_cast<const OverdueTask*>(b.get()) != nullptr;
                  if (aOver != bOver) return aOver;
                  if (a->importance() != b->importance())
                      return a->importance() > b->importance();
                  return a->nextTime() < b->nextTime();
              });
    Plan plan;
    for (auto& t : tasks) plan.push(std::move(t));
    return plan;
}

Scheduler::Stats Scheduler::evaluate(const Plan& plan) {
    Stats s;
    std::uint32_t now = 0;
    for (auto* t : plan.ordered()) {
        if (t->nextTime() < now) {
            ++s.overdueCount;
            s.totalDelaySec += now - t->nextTime();
        }
        now = std::max(now, t->nextTime()) + t->duration();
    }
    return s;
}