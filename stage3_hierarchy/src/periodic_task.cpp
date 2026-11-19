#include "periodic_task.hpp"
#include <stdexcept>

PeriodicTask::PeriodicTask(std::uint32_t id, std::string text, std::string assignee,
                           int importance, std::uint32_t durationSec,
                           std::uint32_t nextRunSec, std::uint32_t periodSec)
    : BaseTask(id, std::move(text), std::move(assignee), importance, durationSec),
      nextRun_(nextRunSec), period_(periodSec)
{
    if (period_ == 0) throw std::invalid_argument("PeriodicTask: period == 0");
}

void PeriodicTask::skipNext() { nextRun_ += period_; }
void PeriodicTask::reschedule() { nextRun_ += period_; }

void PeriodicTask::print(std::ostream& os) const {
    os << "[Periodic #" << id_ << " \"" << text_ << "\" assignee=" << assignee_
       << " imp=" << importance_ << " dur=" << durationSec_
       << "s next=" << nextRun_ << "s period=" << period_ << "s]";
}