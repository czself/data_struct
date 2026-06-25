from __future__ import annotations

from dataclasses import dataclass
from typing import Any

import requests

from .settings import AppSettings


@dataclass(frozen=True)
class AiMessage:
    role: str
    content: str


class AiClientError(RuntimeError):
    pass


def system_prompt() -> str:
    return (
        "你是给小学 3-6 年级孩子讲 Python 的 AI 小老师。"
        "回答必须简短、温和、具体。"
        "不要嘲笑孩子，不要直接给一大段最终答案。"
        "优先解释思路、指出可以尝试的下一步。"
        "如果给代码，只给很小的片段，并说明放在哪里。"
    )


def build_chat_payload(settings: AppSettings, messages: list[AiMessage]) -> dict[str, Any]:
    return {
        "model": settings.model,
        "messages": [{"role": "system", "content": system_prompt()}]
        + [{"role": m.role, "content": m.content} for m in messages],
        "temperature": 0.4,
    }


class OpenAICompatibleClient:
    def __init__(self, settings: AppSettings) -> None:
        self.settings = settings

    def chat(self, messages: list[AiMessage], timeout: int = 60) -> str:
        if not self.settings.api_key.strip():
            raise AiClientError("还没有设置 AI Key。请先在“设置 -> AI 小老师设置”里填写。")
        url = self.settings.base_url.rstrip("/") + "/v1/chat/completions"
        headers = {
            "Authorization": f"Bearer {self.settings.api_key}",
            "Content-Type": "application/json",
        }
        try:
            response = requests.post(
                url,
                headers=headers,
                json=build_chat_payload(self.settings, messages),
                timeout=timeout,
            )
        except requests.RequestException as exc:
            raise AiClientError(f"AI 小老师暂时连不上：{exc}") from exc
        if response.status_code == 401 or response.status_code == 403:
            raise AiClientError("AI Key 好像不对，或者没有权限。请检查后再试。")
        if response.status_code >= 400:
            raise AiClientError(f"AI 小老师返回了错误：HTTP {response.status_code}\n{response.text[:500]}")
        try:
            data = response.json()
            return data["choices"][0]["message"]["content"].strip()
        except (KeyError, IndexError, ValueError, TypeError) as exc:
            raise AiClientError("AI 小老师的回答格式有点奇怪，请稍后再试。") from exc


def explain_selection_messages(code: str) -> list[AiMessage]:
    return [
        AiMessage(
            "user",
            "请用小学生能听懂的话解释这段 Python 代码。"
            "先说它做什么，再用 3 个以内的小步骤解释。\n\n"
            f"代码：\n```python\n{code}\n```",
        )
    ]


def explain_error_messages(code: str, error_text: str) -> list[AiMessage]:
    return [
        AiMessage(
            "user",
            "请解释这个 Python 错误。"
            "先指出可能是哪一行，再说原因，最后给一个小提示，不要直接重写完整程序。\n\n"
            f"代码：\n```python\n{code}\n```\n\n错误：\n```text\n{error_text}\n```",
        )
    ]


def hint_messages(code: str) -> list[AiMessage]:
    return [
        AiMessage(
            "user",
            "请看这份 Python 代码，只给一个温和的小提示，帮助孩子自己继续想。"
            "不要直接给完整答案。\n\n"
            f"代码：\n```python\n{code}\n```",
        )
    ]
