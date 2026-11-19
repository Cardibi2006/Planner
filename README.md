# Планировщик заданий — вариант 9

Приложение имитирует планирование и выполнение заданий в автоматизированной
системе контроля рабочих процессов. Учебный проект (C++ + Python).

## Возможности

- Три типа заданий: **с крайним сроком**, **периодические**, **просроченные**.
- Построение плана жадным алгоритмом (просроченные → по важности → по времени).
- Приоритетная очередь по времени следующего выполнения.
- Получение ближайшего задания и всех заданий указанного исполнителя.
- Графический интерфейс на Python (Tkinter), C++-ядро через `ctypes`.

## Структура

| Папка | Этап | Содержимое |
|---|---|---|
| `stage1_task_class/` | 1 | Класс `Task` на C++ с тестами |
| `stage3_hierarchy/` | 3 | Иерархия `BaseTask` → `DeadlineTask`/`PeriodicTask`/`OverdueTask` |
| `stage5_prototype_ui/` | 5 | Python-прототип UI (Tkinter) |
| `stage6_cpp_container/` | 6 | `Plan` (приоритетная очередь) + `Scheduler` |
| `stage7_binding/` | 7 | `extern "C"` API + Python UI на `ctypes` |

## Сборка

```bash
# Linux/macOS
sudo apt install build-essential cmake python3-tk   # Debian/Ubuntu
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure

python3 stage5_prototype_ui/scheduler_prototype.py
python3 stage7_binding/python/scheduler_app.py
```

## Лицензия
см. [LICENSE](LICENSE).