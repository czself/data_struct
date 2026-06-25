from __future__ import annotations

import json
import os
import sys
from dataclasses import dataclass
from pathlib import Path

from .constants import RUNTIME_PYTHON_DIR


@dataclass(frozen=True)
class PythonCommand:
    program: Path
    args: list[str]
    cwd: Path
    display: str


def bundled_python_executable() -> Path | None:
    exe_name = "python.exe" if os.name == "nt" else "python"
    candidate = RUNTIME_PYTHON_DIR / exe_name
    if candidate.exists():
        return candidate
    env_value = os.environ.get("PORTABLE_PY_IDE_PYTHON")
    if env_value and Path(env_value).exists():
        return Path(env_value)
    return None


def python_executable() -> Path:
    return bundled_python_executable() or Path(sys.executable)


def source_cwd(source_file: Path) -> Path:
    parent = source_file.resolve().parent
    return parent if parent.exists() else Path.cwd()


def check_command(
    source_file: Path,
    python: Path | None = None,
    cwd: Path | None = None,
) -> PythonCommand:
    source_file = source_file.resolve()
    program = python or python_executable()
    args = ["-m", "py_compile", str(source_file)]
    return PythonCommand(
        program=program,
        args=args,
        cwd=(cwd.resolve() if cwd else source_cwd(source_file)),
        display=f"{program} -m py_compile {source_file.name}",
    )


def run_command(
    source_file: Path,
    python: Path | None = None,
    cwd: Path | None = None,
) -> PythonCommand:
    source_file = source_file.resolve()
    program = python or python_executable()
    return PythonCommand(
        program=program,
        args=[str(source_file)],
        cwd=(cwd.resolve() if cwd else source_cwd(source_file)),
        display=f"{program} {source_file.name}",
    )


def debug_command(
    source_file: Path,
    project_root: Path,
    breakpoints: dict[str, list[int]],
    python: Path | None = None,
) -> PythonCommand:
    source_file = source_file.resolve()
    project_root = project_root.resolve()
    program = python or python_executable()
    args = [
        "-m",
        "portable_py_ide.debug_runner",
        "--script",
        str(source_file),
        "--root",
        str(project_root),
        "--breakpoints",
        json.dumps(breakpoints, ensure_ascii=False),
    ]
    return PythonCommand(
        program=program,
        args=args,
        cwd=project_root,
        display=f"{program} -m portable_py_ide.debug_runner {source_file.name}",
    )


def python_env() -> dict[str, str]:
    env = os.environ.copy()
    env["PYTHONUTF8"] = "1"
    env["PYTHONIOENCODING"] = "utf-8"
    return env
