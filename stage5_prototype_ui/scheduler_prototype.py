# stage5_prototype_ui/scheduler_prototype.py
# Этап 5: прототип UI (все классы в одном файле, без C++).
# Запуск: python scheduler_prototype.py
import heapq
import itertools
import tkinter as tk
from tkinter import ttk, messagebox, simpledialog
from dataclasses import dataclass
from typing import List, Optional


@dataclass
class DeadlineTask:
    id: int; text: str; assignee: str; importance: int; duration: int; deadline: int
    kind = "Deadline"
    def next_time(self): return self.deadline

@dataclass
class PeriodicTask:
    id: int; text: str; assignee: str; importance: int; duration: int
    next_run: int; period: int
    kind = "Periodic"
    def next_time(self): return self.next_run

@dataclass
class OverdueTask:
    id: int; text: str; assignee: str; importance: int; duration: int
    kind = "Overdue"
    def next_time(self): return 0


class Plan:
    def __init__(self):
        self.h = []
        self.c = itertools.count()
    def push(self, t):
        heapq.heappush(self.h, (t.next_time(), next(self.c), t))
    def pop(self):
        return heapq.heappop(self.h)[2] if self.h else None
    def ordered(self):
        return [t for _, _, t in sorted(self.h, key=lambda x: (x[0], x[1]))]


def build_plan(tasks):
    plan = Plan()
    for t in sorted(tasks, key=lambda t: (0 if t.kind == "Overdue" else 1,
                                          -t.importance, t.next_time())):
        plan.push(t)
    return plan


class App:
    def __init__(self, root):
        self.root = root
        self.root.title("Планировщик заданий — прототип (вариант 9)")
        self.root.geometry("900x560")
        self.tasks: List = []
        self.next_id = 1
        self.plan: Optional[Plan] = None
        self._ui()
        self._demo()

    def _ui(self):
        f = ttk.LabelFrame(self.root, text="Новое задание", padding=6)
        f.pack(fill="x", padx=8, pady=4)
        self.type_var = tk.StringVar(value="Deadline")
        self.text_var = tk.StringVar()
        self.assignee_var = tk.StringVar(value="Иванов")
        self.imp_var = tk.IntVar(value=5)
        self.dur_var = tk.IntVar(value=600)
        self.p1_var = tk.IntVar(value=3600)
        self.p2_var = tk.IntVar(value=1800)
        ttk.Label(f, text="Тип:").grid(row=0, column=0)
        ttk.Combobox(f, textvariable=self.type_var, width=9, state="readonly",
                     values=["Deadline", "Periodic", "Overdue"]).grid(row=0, column=1, padx=3)
        ttk.Label(f, text="Текст:").grid(row=0, column=2)
        ttk.Entry(f, textvariable=self.text_var, width=20).grid(row=0, column=3, padx=3)
        ttk.Label(f, text="Исполнитель:").grid(row=0, column=4)
        ttk.Entry(f, textvariable=self.assignee_var, width=10).grid(row=0, column=5, padx=3)
        ttk.Label(f, text="Важность:").grid(row=0, column=6)
        ttk.Spinbox(f, from_=0, to=10, textvariable=self.imp_var, width=4).grid(row=0, column=7, padx=3)
        ttk.Label(f, text="Длит.:").grid(row=0, column=8)
        ttk.Entry(f, textvariable=self.dur_var, width=6).grid(row=0, column=9, padx=3)
        ttk.Label(f, text="Срок/Ближ.:").grid(row=0, column=10)
        ttk.Entry(f, textvariable=self.p1_var, width=6).grid(row=0, column=11, padx=3)
        ttk.Label(f, text="Период:").grid(row=0, column=12)
        ttk.Entry(f, textvariable=self.p2_var, width=6).grid(row=0, column=13, padx=3)
        ttk.Button(f, text="Добавить", command=self.add).grid(row=0, column=14, padx=6)

        cols = ("id", "kind", "text", "assignee", "imp", "dur", "p1", "p2")
        self.tree = ttk.Treeview(self.root, columns=cols, show="headings", height=8)
        heads = {"id": "ID", "kind": "Тип", "text": "Текст", "assignee": "Исполнитель",
                 "imp": "Важность", "dur": "Длит.", "p1": "Срок/Ближ.", "p2": "Период"}
        for c in cols:
            self.tree.heading(c, text=heads[c])
            self.tree.column(c, width=80 if c != "text" else 200, anchor="center")
        self.tree.pack(fill="both", expand=True, padx=8, pady=4)

        ops = ttk.Frame(self.root)
        ops.pack(fill="x", padx=8)
        ttk.Button(ops, text="Продлить срок", command=self.extend).pack(side="left", padx=2)
        ttk.Button(ops, text="Изменить важность", command=self.change_imp).pack(side="left", padx=2)
        ttk.Button(ops, text="Пропустить период", command=self.skip).pack(side="left", padx=2)
        ttk.Button(ops, text="Удалить", command=self.delete).pack(side="left", padx=2)

        pf = ttk.LabelFrame(self.root, text="План", padding=4)
        pf.pack(fill="both", expand=True, padx=8, pady=4)
        top = ttk.Frame(pf); top.pack(fill="x")
        ttk.Button(top, text="Построить план", command=self.build).pack(side="left", padx=2)
        ttk.Button(top, text="Выполнить ближайшее", command=self.run_next).pack(side="left", padx=2)
        ttk.Label(top, text="Исполнитель:").pack(side="left", padx=(15, 2))
        self.filter_var = tk.StringVar()
        self.filter_cb = ttk.Combobox(top, textvariable=self.filter_var, state="readonly", width=12)
        self.filter_cb.pack(side="left")
        ttk.Button(top, text="Показать", command=self.show_assignee).pack(side="left", padx=2)

        pcols = ("n", "time", "kind", "id", "text", "assignee", "imp")
        self.ptree = ttk.Treeview(pf, columns=pcols, show="headings", height=7)
        pheads = {"n": "#", "time": "Время", "kind": "Тип", "id": "ID",
                  "text": "Текст", "assignee": "Исполнитель", "imp": "Важность"}
        for c in pcols:
            self.ptree.heading(c, text=pheads[c])
            self.ptree.column(c, width=80 if c != "text" else 220, anchor="center")
        self.ptree.pack(fill="both", expand=True, pady=(4, 0))

        self.info = tk.StringVar(value="Готово")
        ttk.Label(self.root, textvariable=self.info, relief="sunken",
                  anchor="w").pack(fill="x", padx=8, pady=3)

    def _demo(self):
        self.tasks = [
            DeadlineTask(1, "Сдать отчёт", "Иванов", 8, 1800, 3600),
            DeadlineTask(2, "Проверить логи", "Петров", 5, 900, 7200),
            PeriodicTask(3, "Бэкап", "Сидоров", 7, 600, 1800, 3600),
            OverdueTask(4, "Починить сервер", "Иванов", 9, 1200),
        ]
        self.next_id = 5
        self.refresh()

    def add(self):
        text = self.text_var.get().strip()
        assignee = self.assignee_var.get().strip()
        if not text or not assignee:
            messagebox.showwarning("Ошибка", "Текст и исполнитель обязательны"); return
        try:
            imp, dur = int(self.imp_var.get()), int(self.dur_var.get())
            p1, p2 = int(self.p1_var.get()), int(self.p2_var.get())
        except (ValueError, tk.TclError):
            messagebox.showwarning("Ошибка", "Числовые поля должны быть числами"); return
        if not (0 <= imp <= 10) or dur <= 0:
            messagebox.showwarning("Ошибка", "Важность 0..10, длительность > 0"); return
        t = self.type_var.get()
        if t == "Deadline":
            task = DeadlineTask(self.next_id, text, assignee, imp, dur, p1)
        elif t == "Periodic":
            if p2 <= 0:
                messagebox.showwarning("Ошибка", "Период > 0"); return
            task = PeriodicTask(self.next_id, text, assignee, imp, dur, p1, p2)
        else:
            task = OverdueTask(self.next_id, text, assignee, imp, dur)
        self.tasks.append(task); self.next_id += 1
        self.refresh(); self.info.set(f"Добавлено #{task.id} ({task.kind})")

    def selected(self):
        sel = self.tree.selection()
        if not sel:
            messagebox.showinfo("Нет выбора", "Выберите задание в таблице"); return None
        tid = int(self.tree.item(sel[0])["values"][0])
        for t in self.tasks:
            if t.id == tid: return t
        return None

    def extend(self):
        t = self.selected()
        if not t: return
        if not isinstance(t, DeadlineTask):
            messagebox.showinfo("Не применимо", "Только для Deadline"); return
        t.deadline += 600; self.refresh()
        self.info.set(f"Срок #{t.id} → {t.deadline}")

    def change_imp(self):
        t = self.selected()
        if not t: return
        v = simpledialog.askinteger("Важность", "0..10:", parent=self.root, minvalue=0, maxvalue=10)
        if v is None: return
        t.importance = v; self.refresh()
        self.info.set(f"Важность #{t.id} → {v}")

    def skip(self):
        t = self.selected()
        if not t: return
        if not isinstance(t, PeriodicTask):
            messagebox.showinfo("Не применимо", "Только для Periodic"); return
        t.next_run += t.period; self.refresh()
        self.info.set(f"Пропуск #{t.id} → {t.next_run}")

    def delete(self):
        t = self.selected()
        if not t: return
        self.tasks.remove(t); self.refresh()
        self.info.set(f"Удалено #{t.id}")

    def build(self):
        self.plan = build_plan(self.tasks)
        self.filter_cb["values"] = sorted({t.assignee for t in self.tasks})
        self.show_plan(self.plan.ordered())
        self.info.set(f"План: {len(self.plan.h)} заданий")

    def show_plan(self, tasks):
        for r in self.ptree.get_children(): self.ptree.delete(r)
        for i, t in enumerate(tasks, 1):
            self.ptree.insert("", "end", values=(
                i, t.next_time(), t.kind, t.id, t.text, t.assignee, t.importance))

    def run_next(self):
        if not self.plan or not self.plan.h:
            messagebox.showinfo("План пуст", "Сначала постройте план"); return
        t = self.plan.pop()
        if isinstance(t, PeriodicTask):
            t.next_run += t.period
            self.plan.push(t)
            msg = f"Выполнено периодическое #{t.id}; следующее — {t.next_run}"
        else:
            self.tasks.remove(t)
            msg = f"Выполнено #{t.id} ({t.kind})"
        self.refresh(); self.info.set(msg)

    def show_assignee(self):
        if not self.plan:
            messagebox.showinfo("План не построен", "Сначала постройте план"); return
        a = self.filter_var.get()
        self.show_plan([t for t in self.plan.ordered() if t.assignee == a])
        self.info.set(f"Задания «{a}»")

    def refresh(self):
        for r in self.tree.get_children(): self.tree.delete(r)
        for t in self.tasks:
            p1 = getattr(t, "deadline", getattr(t, "next_run", "—"))
            p2 = getattr(t, "period", "—")
            self.tree.insert("", "end", values=(
                t.id, t.kind, t.text, t.assignee, t.importance, t.duration, p1, p2))
        if self.plan is not None:
            self.show_plan(self.plan.ordered())


if __name__ == "__main__":
    root = tk.Tk()
    App(root)
    root.mainloop()