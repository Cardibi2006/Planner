#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlannerHandle PlannerHandle;

PlannerHandle* planner_create(void);
void           planner_destroy(PlannerHandle* h);

unsigned int   planner_add(PlannerHandle* h, int type, const char* text,
                           const char* assignee, int importance,
                           unsigned int duration, unsigned int p1, unsigned int p2);
int            planner_remove(PlannerHandle* h, unsigned int id);
int            planner_set_importance(PlannerHandle* h, unsigned int id, int imp);
int            planner_extend_deadline(PlannerHandle* h, unsigned int id, unsigned int extra);
int            planner_skip_period(PlannerHandle* h, unsigned int id);

unsigned int   planner_build_plan(PlannerHandle* h);
unsigned int   planner_run_next(PlannerHandle* h);

char*          planner_dump_plan(PlannerHandle* h);
char*          planner_dump_assignee(PlannerHandle* h, const char* assignee);
void           planner_free_string(char* s);

#ifdef __cplusplus
}
#endif#include "c_api.hpp"
#include "plan.hpp"
#include "scheduler.hpp"
#include "deadline_task.hpp"
#include "periodic_task.hpp"
#include "overdue_task.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

struct PlannerHandle {
    std::vector<std::unique_ptr<BaseTask>> tasks;
    Plan plan;
    std::uint32_t nextId = 1;
};

extern "C" {

PlannerHandle* planner_create(void) { return new PlannerHandle(); }
void planner_destroy(PlannerHandle* h) { delete h; }

unsigned int planner_add(PlannerHandle* h, int type, const char* text,
                         const char* assignee, int importance,
                         unsigned int duration, unsigned int p1, unsigned int p2) {
    if (!h || !text || !assignee) return 0;
    try {
        std::uint32_t id = h->nextId++;
        std::unique_ptr<BaseTask> t;
        switch (type) {
            case 0: t = std::make_unique<DeadlineTask>(id, text, assignee, importance, duration, p1); break;
            case 1: t = std::make_unique<PeriodicTask>(id, text, assignee, importance, duration, p1, p2); break;
            case 2: t = std::make_unique<OverdueTask>(id, text, assignee, importance, duration); break;
            default: return 0;
        }
        h->tasks.push_back(std::move(t));
        return id;
    } catch (...) { return 0; }
}

int planner_remove(PlannerHandle* h, unsigned int id) {
    if (!h) return 0;
    auto it = std::find_if(h->tasks.begin(), h->tasks.end(),
        [id](const auto& p){ return p->id() == id; });
    if (it == h->tasks.end()) return 0;
    h->tasks.erase(it);
    return 1;
}

int planner_set_importance(PlannerHandle* h, unsigned int id, int imp) {
    if (!h) return 0;
    for (auto& p : h->tasks) if (p->id() == id) {
        try { p->setImportance(imp); return 1; } catch (...) { return 0; }
    }
    return 0;
}

int planner_extend_deadline(PlannerHandle* h, unsigned int id, unsigned int extra) {
    if (!h) return 0;
    for (auto& p : h->tasks) if (p->id() == id) {
        if (auto* d = dynamic_cast<DeadlineTask*>(p.get())) { d->extendDeadline(extra); return 1; }
        return 0;
    }
    return 0;
}

int planner_skip_period(PlannerHandle* h, unsigned int id) {
    if (!h) return 0;
    for (auto& p : h->tasks) if (p->id() == id) {
        if (auto* pp = dynamic_cast<PeriodicTask*>(p.get())) { pp->skipNext(); return 1; }
        return 0;
    }
    return 0;
}

unsigned int planner_build_plan(PlannerHandle* h) {
    if (!h) return 0;
    std::vector<std::unique_ptr<BaseTask>> moved;
    moved.reserve(h->tasks.size());
    for (auto& p : h->tasks) moved.push_back(std::move(p));
    h->plan = Scheduler::build(std::move(moved));
    h->tasks.clear();
    return static_cast<unsigned int>(h->plan.size());
}

unsigned int planner_run_next(PlannerHandle* h) {
    if (!h || h->plan.empty()) return 0;
    auto t = h->plan.pop();
    if (!t) return 0;
    auto id = t->id();
    if (auto* p = dynamic_cast<PeriodicTask*>(t.get())) {
        p->reschedule();
        h->plan.push(std::move(t));
    }
    return id;
}

char* planner_dump_plan(PlannerHandle* h) {
    std::ostringstream os;
    if (h) {
        int i = 1;
        for (auto* t : h->plan.ordered()) os << i++ << ". " << *t << "\n";
    }
    auto s = os.str();
    char* out = static_cast<char*>(std::malloc(s.size() + 1));
    std::memcpy(out, s.c_str(), s.size() + 1);
    return out;
}

char* planner_dump_assignee(PlannerHandle* h, const char* assignee) {
    std::ostringstream os;
    if (h && assignee) {
        for (auto* t : h->plan.forAssignee(assignee)) os << *t << "\n";
    }
    auto s = os.str();
    char* out = static_cast<char*>(std::malloc(s.size() + 1));
    std::memcpy(out, s.c_str(), s.size() + 1);
    return out;
}

void planner_free_string(char* s) { std::free(s); }

} // extern "C"