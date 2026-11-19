#pragma once
#include "base_task.hpp"
#include <memory>
#include <queue>
#include <string>
#include <vector>

class Plan {
public:
    Plan() = default;
    Plan(const Plan&) = delete;
    Plan& operator=(const Plan&) = delete;
    Plan(Plan&&) = default;
    Plan& operator=(Plan&&) = default;

    void push(std::unique_ptr<BaseTask> t);
    std::unique_ptr<BaseTask> pop();
    const BaseTask* peek() const;
    bool empty() const;
    std::size_t size() const;

    std::vector<const BaseTask*> ordered() const;
    std::vector<const BaseTask*> forAssignee(const std::string& a) const;

private:
    struct Cmp {
        bool operator()(const std::unique_ptr<BaseTask>& a,
                        const std::unique_ptr<BaseTask>& b) const {
            return a->nextTime() > b->nextTime();
        }
    };
    std::priority_queue<std::unique_ptr<BaseTask>,
                        std::vector<std::unique_ptr<BaseTask>>, Cmp> q_;
};