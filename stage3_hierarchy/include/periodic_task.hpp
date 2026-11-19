#pragma once
#include "base_task.hpp"

class PeriodicTask : public BaseTask {
public:
    PeriodicTask(std::uint32_t id, std::string text, std::string assignee,
                 int importance, std::uint32_t durationSec,
                 std::uint32_t nextRunSec, std::uint32_t periodSec);

    std::uint32_t nextTime() const override { return nextRun_; }
    void print(std::ostream& os) const override;

    void skipNext();
    void reschedule();

    std::uint32_t period() const noexcept { return period_; }
    std::uint32_t nextRun() const noexcept { return nextRun_; }

private:
    std::uint32_t nextRun_;
    std::uint32_t period_;
};