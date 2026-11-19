#pragma once
#include <compare>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

class BaseTask {
public:
    BaseTask(std::uint32_t id, std::string text, std::string assignee,
             int importance, std::uint32_t durationSec);
    virtual ~BaseTask() = default;

    std::uint32_t id() const noexcept { return id_; }
    const std::string& text() const noexcept { return text_; }
    const std::string& assignee() const noexcept { return assignee_; }
    int importance() const noexcept { return importance_; }
    std::uint32_t duration() const noexcept { return durationSec_; }

    void setImportance(int v);
    void setAssignee(std::string a);

    virtual std::uint32_t nextTime() const = 0;
    virtual void print(std::ostream& os) const;

    std::strong_ordering operator<=>(const BaseTask& o) const noexcept {
        return importance_ <=> o.importance_;
    }
    bool operator==(const BaseTask& o) const noexcept {
        return importance_ == o.importance_;
    }

protected:
    std::uint32_t id_;
    std::string text_;
    std::string assignee_;
    int importance_;
    std::uint32_t durationSec_;
};

std::ostream& operator<<(std::ostream& os, const BaseTask& t);