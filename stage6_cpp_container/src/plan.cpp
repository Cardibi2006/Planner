#include "plan.hpp"
#include <algorithm>

void Plan::push(std::unique_ptr<BaseTask> t) { q_.push(std::move(t)); }

std::unique_ptr<BaseTask> Plan::pop() {
    if (q_.empty()) return nullptr;
    auto t = std::move(const_cast<std::unique_ptr<BaseTask>&>(q_.top()));
    q_.pop();
    return t;
}

const BaseTask* Plan::peek() const { return q_.empty() ? nullptr : q_.top().get(); }
bool Plan::empty() const { return q_.empty(); }
std::size_t Plan::size() const { return q_.size(); }

std::vector<const BaseTask*> Plan::ordered() const {
    auto copy = q_;
    std::vector<const BaseTask*> result;
    while (!copy.empty()) { result.push_back(copy.top().get()); copy.pop(); }
    return result;
}

std::vector<const BaseTask*> Plan::forAssignee(const std::string& a) const {
    auto all = ordered();
    std::vector<const BaseTask*> r;
    for (auto* t : all) if (t->assignee() == a) r.push_back(t);
    return r;
}