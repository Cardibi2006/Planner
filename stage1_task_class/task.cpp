// stage1_task_class/task.cpp — этап 1: класс Task (планировщик заданий), C++20
// Сборка отдельно: g++ -std=c++20 -Wall -Wextra -O2 -o task task.cpp && ./task
#include <algorithm>
#include <compare>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

enum class TaskStatus : std::uint8_t {
    Pending = 0, Running = 1, Done = 2, Cancelled = 3
};

inline const char* statusToString(TaskStatus s) {
    switch (s) {
        case TaskStatus::Pending:   return "Pending";
        case TaskStatus::Running:   return "Running";
        case TaskStatus::Done:      return "Done";
        case TaskStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

class Task {
public:
    Task(std::uint32_t id, std::string name, int priority,
         std::uint32_t durationMs, std::uint32_t deadlineMs = 0)
        : id_(id), name_(std::move(name)), priority_(priority),
          durationMs_(durationMs), deadlineMs_(deadlineMs)
    {
        if (id_ == 0) throw std::invalid_argument("Task: id == 0");
        if (name_.empty()) throw std::invalid_argument("Task: empty name");
        if (priority_ < 0 || priority_ > 10) throw std::invalid_argument("Task: priority out of [0..10]");
        if (durationMs_ == 0) throw std::invalid_argument("Task: duration == 0");
    }

    std::uint32_t id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
    int priority() const noexcept { return priority_; }
    std::uint32_t durationMs() const noexcept { return durationMs_; }
    std::uint32_t deadlineMs() const noexcept { return deadlineMs_; }
    TaskStatus status() const noexcept { return status_; }
    const std::vector<std::uint32_t>& dependencies() const noexcept { return dependencies_; }

    void setPriority(int p) {
        if (p < 0 || p > 10) throw std::invalid_argument("Task::setPriority");
        priority_ = p;
    }
    void setStatus(TaskStatus s) noexcept { status_ = s; }

    bool addDependency(std::uint32_t depId) {
        if (depId == 0) throw std::invalid_argument("Task::addDependency: 0");
        if (depId == id_) throw std::invalid_argument("Task::addDependency: self");
        if (std::find(dependencies_.begin(), dependencies_.end(), depId) != dependencies_.end())
            return false;
        dependencies_.push_back(depId);
        return true;
    }
    bool removeDependency(std::uint32_t depId) {
        auto it = std::find(dependencies_.begin(), dependencies_.end(), depId);
        if (it == dependencies_.end()) return false;
        dependencies_.erase(it);
        return true;
    }

    std::strong_ordering operator<=>(const Task& o) const noexcept { return priority_ <=> o.priority_; }
    bool operator==(const Task& o) const noexcept { return priority_ == o.priority(); }

    friend std::ostream& operator<<(std::ostream& os, const Task& t) {
        os << "[Task #" << t.id_ << " \"" << t.name_ << "\""
           << " prio=" << t.priority_ << " dur=" << t.durationMs_ << "ms"
           << " deadline=" << t.deadlineMs_ << "ms status=" << statusToString(t.status_);
        if (!t.dependencies_.empty()) {
            os << " deps=[";
            for (std::size_t i = 0; i < t.dependencies_.size(); ++i) {
                if (i) os << ",";
                os << t.dependencies_[i];
            }
            os << "]";
        }
        return os << "]";
    }

private:
    std::uint32_t id_;
    std::string name_;
    int priority_;
    std::uint32_t durationMs_;
    std::uint32_t deadlineMs_;
    TaskStatus status_ = TaskStatus::Pending;
    std::vector<std::uint32_t> dependencies_;
};

namespace test {
int g_checks = 0, g_failed = 0;
void check(bool c, const char* w, int line) {
    ++g_checks;
    if (!c) { ++g_failed; std::cerr << "FAIL line " << line << ": " << w << "\n"; }
}
#define CHECK(c) ::test::check((c), #c, __LINE__)
template <class Ex, class F> void checkThrows(F&& f, const char* w, int line) {
    ++g_checks;
    try { f(); ++g_failed; std::cerr << "FAIL line " << line << ": expected throw: " << w << "\n"; }
    catch (const Ex&) {}
    catch (...) { ++g_failed; std::cerr << "FAIL line " << line << ": wrong exception\n"; }
}
#define CHECK_THROWS(Ex, expr) ::test::checkThrows<Ex>([&]{ expr; }, #expr, __LINE__)

void runAll() {
    Task t1(1, "Build report", 5, 1200, 5000);
    CHECK(t1.id() == 1);
    CHECK(t1.name() == "Build report");
    CHECK(t1.priority() == 5);
    CHECK(t1.durationMs() == 1200);
    CHECK(t1.deadlineMs() == 5000);
    CHECK(t1.status() == TaskStatus::Pending);

    CHECK_THROWS(std::invalid_argument, Task(0, "x", 1, 1));
    CHECK_THROWS(std::invalid_argument, Task(2, "", 1, 1));
    CHECK_THROWS(std::invalid_argument, Task(3, "x", -1, 1));
    CHECK_THROWS(std::invalid_argument, Task(4, "x", 11, 1));
    CHECK_THROWS(std::invalid_argument, Task(5, "x", 1, 0));

    t1.setPriority(9);
    CHECK(t1.priority() == 9);
    CHECK_THROWS(std::invalid_argument, t1.setPriority(42));

    t1.setStatus(TaskStatus::Running);
    CHECK(t1.status() == TaskStatus::Running);

    Task t2(2, "Test report", 7, 800);
    Task t3(3, "Publish", 3, 400);
    CHECK(t2.addDependency(1) == true);
    CHECK(t2.addDependency(3) == true);
    CHECK(t2.addDependency(1) == false);
    CHECK_THROWS(std::invalid_argument, t2.addDependency(0));
    CHECK_THROWS(std::invalid_argument, t2.addDependency(2));
    CHECK(t2.removeDependency(1) == true);
    CHECK(t2.removeDependency(1) == false);

    CHECK(t2 < t1);
    CHECK(!(t1 < t2));
    CHECK(t2 == Task(99, "other", 7, 1));
    CHECK(t1 > t3);

    std::vector<Task> v{t3, t1, t2};
    std::sort(v.begin(), v.end(), std::greater<>());
    CHECK(v[0].priority() == 9);
    CHECK(v[1].priority() == 7);
    CHECK(v[2].priority() == 3);

    std::ostringstream oss;
    oss << t2;
    CHECK(oss.str().find("#2") != std::string::npos);
}
#undef CHECK
#undef CHECK_THROWS
} // namespace test

int main() {
    std::cout << "=== Task demo ===\n";
    Task a(1, "Build report", 5, 1200, 5000);
    Task b(2, "Test report", 7, 800);
    b.addDependency(1);
    b.setStatus(TaskStatus::Running);
    std::cout << a << "\n" << b << "\n";
    std::cout << "\n=== Tests ===\n";
    test::runAll();
    std::cout << "Checks: " << test::g_checks << ", failed: " << test::g_failed << "\n";
    return test::g_failed == 0 ? 0 : 1;
}