#include "plan.hpp"
#include "scheduler.hpp"
#include "deadline_task.hpp"
#include "periodic_task.hpp"
#include "overdue_task.hpp"
#include <iostream>

static int g_checks = 0, g_failed = 0;
#define CHECK(c) do { ++g_checks; if (!(c)) { ++g_failed; \
    std::cerr << "FAIL line " << __LINE__ << ": " #c "\n"; } } while (0)

int main() {
    std::vector<std::unique_ptr<BaseTask>> tasks;
    tasks.push_back(std::make_unique<DeadlineTask>(1, "Отчёт", "Иванов", 8, 1800, 3600));
    tasks.push_back(std::make_unique<PeriodicTask>(2, "Бэкап", "Сидоров", 7, 600, 1800, 3600));
    tasks.push_back(std::make_unique<OverdueTask>(3, "Починить", "Иванов", 9, 1200));

    auto plan = Scheduler::build(std::move(tasks));
    CHECK(plan.size() == 3);
    CHECK(plan.peek() != nullptr);
    CHECK(plan.peek()->nextTime() == 0);
    CHECK(plan.peek()->id() == 3);

    auto ivanov = plan.forAssignee("Иванов");
    CHECK(ivanov.size() == 2);

    auto t1 = plan.pop();
    CHECK(t1 != nullptr && t1->id() == 3);
    auto t2 = plan.pop();
    CHECK(t2 != nullptr && t2->id() == 2);
    auto t3 = plan.pop();
    CHECK(t3 != nullptr && t3->id() == 1);
    CHECK(plan.empty());

    std::cout << "Checks: " << g_checks << ", failed: " << g_failed << "\n";
    return g_failed ? 1 : 0;
}