#pragma once
#include "base_task.hpp"

class OverdueTask : public BaseTask {
public:
    OverdueTask(std::uint32_t id, std::string text, std::string assignee,
                int importance, std::uint32_t durationSec);

    std::uint32_t nextTime() const override { return 0; }
    void print(std::ostream& os) const override;
};