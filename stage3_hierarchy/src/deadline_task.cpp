#include "deadline_task.hpp"
#include <stdexcept>

DeadlineTask::DeadlineTask(std::uint32_t id, std::string text, std::string assignee,
                           int importance, std::uint32_t durationSec,
                           std::uint32_t deadlineSec)
    : BaseTask(id, std::move(text), std::move(assignee), importance, durationSec),
      deadline_(deadlineSec)
{
    if (deadline_ < durationSec_)
        throw std::invalid_argument("DeadlineTask: deadline < duration");
}

void DeadlineTask::extendDeadline(std::uint32_t extraSec) {
    deadline_ += extraSec;
}

void DeadlineTask::print(std::ostream& os) const {
    os << "[Deadline #" << id_ << " \"" << text_ << "\" assignee=" << assignee_
       << " imp=" << importance_ << " dur=" << durationSec_
       << "s deadline=" << deadline_ << "s]";
}