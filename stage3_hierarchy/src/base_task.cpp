#include "base_task.hpp"
#include <stdexcept>

BaseTask::BaseTask(std::uint32_t id, std::string text, std::string assignee,
                   int importance, std::uint32_t durationSec)
    : id_(id), text_(std::move(text)), assignee_(std::move(assignee)),
      importance_(importance), durationSec_(durationSec)
{
    if (id_ == 0) throw std::invalid_argument("BaseTask: id == 0");
    if (text_.empty()) throw std::invalid_argument("BaseTask: empty text");
    if (assignee_.empty()) throw std::invalid_argument("BaseTask: empty assignee");
    if (importance_ < 0 || importance_ > 10) throw std::invalid_argument("BaseTask: importance");
    if (durationSec_ == 0) throw std::invalid_argument("BaseTask: duration == 0");
}

void BaseTask::setImportance(int v) {
    if (v < 0 || v > 10) throw std::invalid_argument("BaseTask::setImportance");
    importance_ = v;
}

void BaseTask::setAssignee(std::string a) {
    if (a.empty()) throw std::invalid_argument("BaseTask::setAssignee");
    assignee_ = std::move(a);
}

void BaseTask::print(std::ostream& os) const {
    os << "[#" << id_ << " \"" << text_ << "\" assignee=" << assignee_
       << " imp=" << importance_ << " dur=" << durationSec_ << "s]";
}

std::ostream& operator<<(std::ostream& os, const BaseTask& t) {
    t.print(os);
    return os;
}