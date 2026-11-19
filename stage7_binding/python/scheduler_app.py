# Графический UI (Tkinter) поверх C++-ядра через ctypes.
# Запуск: python scheduler_app.py
import tkinter as tk
from tkinter import ttk, messagebox
from scheduler_core import PlannerCore


class App:
    def __init__(self, root):
        self.root = root
        self.root.title("Планировщик заданий — вариант 9 (C++ core)")
        self.root.geometry("950x600")
        try:
            self.core = PlannerCore()
        except FileNotFoundError as e:
            messagebox.showerror("Библиотека не найдена",
                                 f"{e}\n\nСоберите проект через cmake.")
            raise SystemExit(1)
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

        pf = ttk.LabelFrame(self.root, text="План (C++ core)", padding=4)
        pf.pack(fill="both", expand=True, padx=8, pady=4)
        top = ttk.Frame(pf); top.pack(fill="x")
        ttk.Button(top, text="Построить план", command=self.build).pack(side="left", padx=2)
        ttk.Button(top, text="Выполнить ближайшее", command=self.run_next).pack(side="left", padx=2)
        ttk.Label(top, text="Исполнитель:").pack(side="left", padx=(15, 2))
        self.filter_var = tk.StringVar()
        self.filter_cb = ttk.Combobox(top, textvariable=self.filter_var,
                                      state="readonly", width=12)
        self.filter_cb.pack(side="left")
        ttk.Button(top, text="Показать", command=self.show_assignee).pack(side="left", padx=2)

        self.text_out = tk.Text(pf, height=18, wrap="none")
        self.text_out.pack(fill="both", expand=True, pady=(4, 0))

        self.info = tk.StringVar(value="Готово")
        ttk.Label(self.root, textvariable=self.info, relief="sunken",
                  anchor="w").pack(fill="x", padx=8, pady=3)

    def _demo(self):
        self.core.add("Deadline", "Сдать отчёт", "Иванов", 8, 1800, 3600)
        self.core.add("Deadline", "Проверить логи", "Петров", 5, 900, 7200)
        self.core.add("Periodic", "Бэкап", "Сидоров", 7, 600, 1800, 3600)
        self.core.add("Overdue", "Починить сервер", "Иванов", 9, 1200)
        self.build()

    def add(self):
        try:
            imp = int(self.imp_var.get()); dur = int(self.dur_var.get())
            p1 = int(self.p1_var.get()); p2 = int(self.p2_var.get())
        except (ValueError, tk.TclError):
            messagebox.showwarning("Ошибка", "Числовые поля"); return
        id_ = self.core.add(self.type_var.get(), self.text_var.get().strip(),
                            self.assignee_var.get().strip(), imp, dur, p1, p2)
        if id_ == 0:
            messagebox.showwarning("Ошибка", "Не удалось добавить задание"); return
        self.info.set(f"Добавлено #{id_}")
        self.build()

    def build(self):
        self.core.build_plan()
        self.text_out.delete("1.0", "end")
        self.text_out.insert("end", self.core.dump_plan())
        self.filter_cb["values"] = ["Иванов", "Петров", "Сидоров"]
        self.info.set("План построен")

    def run_next(self):
        id_ = self.core.run_next()
        if id_ == 0:
            messagebox.showinfo("План пуст", "Нечего выполнять"); return
        self.core.build_plan()
        self.text_out.delete("1.0", "end")
        self.text_out.insert("end", self.core.dump_plan())
        self.info.set(f"Выполнено #{id_}")

    def show_assignee(self):
        a = self.filter_var.get()
        if not a: return
        self.text_out.delete("1.0", "end")
        self.text_out.insert("end", self.core.dump_assignee(a))
        self.info.set(f"Задания «{a}»")


if __name__ == "__main__":
    root = tk.Tk()
    App(root)
    root.mainloop()