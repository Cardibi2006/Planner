#include "base_task.hpp"
#include "deadline_task.hpp"
#include "periodic_task.hpp"
#include "overdue_task.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

static int g_checks = 0, g_failed = 0;
#define CHECK(c) do { ++g_checks; if (!(c)) { ++g_failed; \
    std::cerr << "FAIL line " << __LINE__ << ": " #c "\n"; } } while (0)

int main() {
    DeadlineTask d(1, "Отчёт", "Иванов", 8, 1800, 3600);
    PeriodicTask p(2, "Бэкап", "Сидоров", 7, 600, 1800, 3600);
    OverdueTask o(3, "Починить", "Иванов", 9, 1200);

    CHECK(d.nextTime() == 3600);
    CHECK(p.nextTime() == 1800);
    CHECK(o.nextTime() == 0);

    d.extendDeadline(600);
    CHECK(d.nextTime() == 4200);

    p.skipNext();
    CHECK(p.nextTime() == 5400);

    std::vector<std::unique_ptr<BaseTask>> v;
    v.push_back(std::make_unique<DeadlineTask>(10, "A", "X", 5, 100, 200));
    v.push_back(std::make_unique<PeriodicTask>(11, "B", "Y", 6, 100, 300, 50));
    v.push_back(std::make_unique<OverdueTask>(12, "C", "Z", 7, 100));

    std::ostringstream oss;
    for (auto& t : v) oss << *t << "\n";
    CHECK(oss.str().find("Deadline") != std::string::npos);
    CHECK(oss.str().find("Periodic") != std::string::npos);
    CHECK(oss.str().find("Overdue") != std::string::npos);

    CHECK(d < o);
    CHECK(!(o < d));
    d.setImportance(10);
    CHECK(d > o);

    std::cout << "Checks: " << g_checks << ", failed: " << g_failed << "\n";
    return g_failed ? 1 : 0;
}