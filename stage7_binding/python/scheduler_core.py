# ctypes-обёртка над C++-библиотекой libplanner.so / planner.dll
import ctypes
import os
import sys


def _find_library() -> str:
    names = {
        "linux":  ["libplanner.so"],
        "darwin": ["libplanner.dylib"],
        "win32":  ["planner.dll"],
    }.get(sys.platform, ["libplanner.so"])
    here = os.path.dirname(os.path.abspath(__file__))
    roots = [here,
             os.path.abspath(os.path.join(here, "..", "..")),
             os.path.abspath(os.path.join(here, "..", "..", "build")),
             os.path.abspath(os.path.join(here, "..", "..", "build", "stage7_binding"))]
    for root in roots:
        for n in names:
            p = os.path.join(root, n)
            if os.path.exists(p):
                return p
    for n in names:
        try:
            ctypes.CDLL(n); return n
        except OSError:
            pass
    raise FileNotFoundError(f"planner library not found; tried {names}")


class PlannerCore:
    def __init__(self, lib_path: str | None = None):
        self.lib = ctypes.CDLL(lib_path or _find_library())
        L = self.lib
        L.planner_create.restype = ctypes.c_void_p
        L.planner_destroy.argtypes = [ctypes.c_void_p]
        L.planner_add.argtypes = [ctypes.c_void_p, ctypes.c_int,
                                  ctypes.c_char_p, ctypes.c_char_p,
                                  ctypes.c_int, ctypes.c_uint,
                                  ctypes.c_uint, ctypes.c_uint]
        L.planner_add.restype = ctypes.c_uint
        L.planner_remove.argtypes = [ctypes.c_void_p, ctypes.c_uint]
        L.planner_remove.restype = ctypes.c_int
        L.planner_set_importance.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_int]
        L.planner_extend_deadline.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint]
        L.planner_skip_period.argtypes = [ctypes.c_void_p, ctypes.c_uint]
        L.planner_build_plan.argtypes = [ctypes.c_void_p]
        L.planner_build_plan.restype = ctypes.c_uint
        L.planner_run_next.argtypes = [ctypes.c_void_p]
        L.planner_run_next.restype = ctypes.c_uint
        L.planner_dump_plan.argtypes = [ctypes.c_void_p]
        L.planner_dump_plan.restype = ctypes.c_void_p
        L.planner_dump_assignee.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        L.planner_dump_assignee.restype = ctypes.c_void_p
        L.planner_free_string.argtypes = [ctypes.c_void_p]
        self._h = L.planner_create()

    def __del__(self):
        if getattr(self, "_h", None):
            self.lib.planner_destroy(self._h)

    def add(self, kind: str, text: str, assignee: str, importance: int,
            duration: int, p1: int = 0, p2: int = 0) -> int:
        t = {"Deadline": 0, "Periodic": 1, "Overdue": 2}[kind]
        return self.lib.planner_add(self._h, t, text.encode(), assignee.encode(),
                                    importance, duration, p1, p2)

    def remove(self, id_: int) -> int: return self.lib.planner_remove(self._h, id_)
    def set_importance(self, id_: int, imp: int) -> int:
        return self.lib.planner_set_importance(self._h, id_, imp)
    def extend_deadline(self, id_: int, extra: int) -> int:
        return self.lib.planner_extend_deadline(self._h, id_, extra)
    def skip_period(self, id_: int) -> int:
        return self.lib.planner_skip_period(self._h, id_)
    def build_plan(self) -> int: return self.lib.planner_build_plan(self._h)
    def run_next(self) -> int: return self.lib.planner_run_next(self._h)

    def dump_plan(self) -> str:
        p = self.lib.planner_dump_plan(self._h)
        s = ctypes.string_at(p).decode()
        self.lib.planner_free_string(p)
        return s

    def dump_assignee(self, a: str) -> str:
        p = self.lib.planner_dump_assignee(self._h, a.encode())
        s = ctypes.string_at(p).decode()
        self.lib.planner_free_string(p)
        return s