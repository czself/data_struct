from __future__ import annotations

import argparse
import bdb
import json
import os
import runpy
import sys
from pathlib import Path
from reprlib import repr as safe_repr

PROTOCOL_PREFIX = "__PYIDE_DEBUG__ "


def emit(event: dict) -> None:
    print(PROTOCOL_PREFIX + json.dumps(event, ensure_ascii=False), file=sys.stderr, flush=True)


def short_value(value) -> str:
    if isinstance(value, (int, float, bool, str, list, tuple, dict, set, type(None))):
        return safe_repr(value)
    return f"<{type(value).__name__}>"


def simple_locals(frame) -> dict[str, str]:
    result = {}
    for name, value in frame.f_locals.items():
        if name.startswith("__") and name.endswith("__"):
            continue
        result[name] = short_value(value)
        if len(result) >= 40:
            result["..."] = "变量太多，先显示前 40 个"
            break
    return result


class KidDebugger(bdb.Bdb):
    def __init__(self, project_root: Path) -> None:
        super().__init__()
        self.project_root = project_root.resolve()
        self.curframe = None
        self.step_frame = None
        self.step_line = 0

    def run_script(self, script: Path) -> None:
        globals_dict = {
            "runpy": runpy,
            "script": script,
            "__name__": "__debug_runner__",
        }
        code = compile("runpy.run_path(str(script), run_name='__main__')", "<debug-runner>", "exec")
        self.reset()
        sys.settrace(self.trace_dispatch)
        try:
            exec(code, globals_dict, globals_dict)
        finally:
            self.quitting = True
            sys.settrace(None)

    def trace_dispatch(self, frame, event, arg):  # noqa: ANN001 - bdb API
        if self.quitting:
            return None
        if event == "line":
            return self.dispatch_line(frame)
        return self.trace_dispatch

    def dispatch_line(self, frame):  # noqa: ANN001 - bdb API
        filename = Path(frame.f_code.co_filename).resolve()
        if self._inside_project(filename) and (
            self.break_here(frame) or self._should_stop_for_step(frame)
        ):
            self.user_line(frame)
            if self.quitting:
                raise bdb.BdbQuit
        return self.trace_dispatch

    def user_line(self, frame) -> None:
        filename = Path(frame.f_code.co_filename).resolve()
        self.curframe = frame
        emit(
            {
                "type": "stop",
                "file": str(filename),
                "line": frame.f_lineno,
                "locals": simple_locals(frame),
            }
        )
        self._wait_for_command()

    def user_exception(self, frame, exc_info) -> None:
        filename = Path(frame.f_code.co_filename).resolve()
        exc_type, exc, _tb = exc_info
        emit(
            {
                "type": "exception",
                "file": str(filename),
                "line": frame.f_lineno,
                "name": getattr(exc_type, "__name__", str(exc_type)),
                "message": str(exc),
                "locals": simple_locals(frame),
            }
        )

    def _inside_project(self, filename: Path) -> bool:
        try:
            filename.relative_to(self.project_root)
            return True
        except ValueError:
            return False

    def _wait_for_command(self) -> None:
        while True:
            raw = sys.stdin.readline()
            if raw == "":
                self.set_quit()
                raise bdb.BdbQuit
            command = raw.strip().lower()
            if command in {"continue", "c"}:
                self.step_frame = None
                self.step_line = 0
                return
            if command in {"step", "s", "next", "n"}:
                self.step_frame = self.curframe
                self.step_line = self.curframe.f_lineno
                return
            if command in {"stop", "quit", "q"}:
                self.set_quit()
                raise bdb.BdbQuit

    def _should_stop_for_step(self, frame) -> bool:
        return self.step_frame is frame and frame.f_lineno != self.step_line


def load_breakpoints(raw: str) -> dict[Path, list[int]]:
    if not raw:
        return {}
    data = json.loads(raw)
    result: dict[Path, list[int]] = {}
    for file_name, lines in data.items():
        if not isinstance(lines, list):
            continue
        clean = [int(line) for line in lines if int(line) > 0]
        if clean:
            result[Path(file_name).resolve()] = clean
    return result


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--script", required=True)
    parser.add_argument("--root", required=True)
    parser.add_argument("--breakpoints", default="{}")
    args = parser.parse_args(argv)

    script = Path(args.script).resolve()
    project_root = Path(args.root).resolve()
    os.chdir(project_root)
    sys.path.insert(0, str(project_root))

    debugger = KidDebugger(project_root)
    for filename, lines in load_breakpoints(args.breakpoints).items():
        for line in lines:
            try:
                debugger.set_break(str(filename), line)
            except (bdb.BdbError, OSError):
                emit({"type": "message", "message": f"第 {line} 行不能设置断点"})

    emit({"type": "started", "file": str(script)})
    try:
        debugger.run_script(script)
    except bdb.BdbQuit:
        emit({"type": "finished", "reason": "stopped"})
        return 0
    except SystemExit as exc:
        code = exc.code if isinstance(exc.code, int) else 0
        emit({"type": "finished", "reason": "system_exit", "code": code})
        return code
    except Exception as exc:
        emit({"type": "crash", "name": type(exc).__name__, "message": str(exc)})
        raise
    emit({"type": "finished", "reason": "done"})
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
