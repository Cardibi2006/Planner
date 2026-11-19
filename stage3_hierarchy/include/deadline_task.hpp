#pragma once
#include "base_task.hpp"

class DeadlineTask : public BaseTask {
public:
    DeadlineTask(std::uint32_t id, std::string text, std::string assignee,
                 int importance, std::uint32_t durationSec,
                 std::uint32_t deadlineSec);

    std::uint32_t nextTime() const override { return deadline_; }
    void print(std::ostream& os) const override;

    void extendDeadline(std::uint32_t extraSec);
    std::uint32_t deadline() const noexcept { return deadline_; }

private:
    std::uint32_t deadline_;
};