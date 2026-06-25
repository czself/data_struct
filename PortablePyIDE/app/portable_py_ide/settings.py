from __future__ import annotations

import json
import os
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any

from .constants import (
    DEFAULT_BASE_URL,
    DEFAULT_MODEL,
    LEGACY_DEFAULT_BASE_URL,
    LEGACY_DEFAULT_MODEL,
)


def settings_dir() -> Path:
    base = os.environ.get("APPDATA") or os.environ.get("USERPROFILE") or str(Path.home())
    path = Path(base) / "PortablePyIDE"
    path.mkdir(parents=True, exist_ok=True)
    return path


def settings_path() -> Path:
    return settings_dir() / "settings.json"


@dataclass
class AppSettings:
    base_url: str = DEFAULT_BASE_URL
    model: str = DEFAULT_MODEL
    api_key: str = ""
    code_hints_enabled: bool = True
    last_folder: str = ""

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> "AppSettings":
        settings = cls()
        for key in asdict(settings):
            if key in data:
                setattr(settings, key, data[key])
        if settings.base_url == LEGACY_DEFAULT_BASE_URL:
            settings.base_url = DEFAULT_BASE_URL
        if settings.model == LEGACY_DEFAULT_MODEL:
            settings.model = DEFAULT_MODEL
        return settings


def load_settings() -> AppSettings:
    path = settings_path()
    if not path.exists():
        return AppSettings()
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return AppSettings()
    if not isinstance(data, dict):
        return AppSettings()
    return AppSettings.from_dict(data)


def save_settings(settings: AppSettings) -> None:
    path = settings_path()
    path.write_text(
        json.dumps(asdict(settings), ensure_ascii=False, indent=2),
        encoding="utf-8",
    )


def masked_api_key(api_key: str) -> str:
    if not api_key:
        return "未设置"
    if len(api_key) <= 8:
        return "*" * len(api_key)
    return f"{api_key[:4]}...{api_key[-4:]}"
