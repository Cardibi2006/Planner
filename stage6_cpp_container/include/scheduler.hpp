#pragma once
#include "plan.hpp"
#include <cstdint>
#include <memory>
#include <vector>

class Scheduler {
public:
    static Plan build(std::vector<std::unique_ptr<BaseTask>> tasks);

    struct Stats {
        std::size_t overdueCount = 0;
        std::uint32_t totalDelaySec = 0;
    };
    static Stats evaluate(const Plan& plan);
};