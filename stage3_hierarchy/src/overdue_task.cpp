#include "overdue_task.hpp"

OverdueTask::OverdueTask(std::uint32_t id, std::string text, std::string assignee,
                         int importance, std::uint32_t durationSec)
    : BaseTask(id, std::move(text), std::move(assignee), importance, durationSec) {}

void OverdueTask::print(std::ostream& os) const {
    os << "[Overdue #" << id_ << " \"" << text_ << "\" assignee=" << assignee_
       << " imp=" << importance_ << " dur=" << durationSec_ << "s]";
}