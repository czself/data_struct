@echo off
setlocal
chcp 65001 >nul
set PYTHONUTF8=1
set PYTHONIOENCODING=utf-8
cd /d "%~dp0"
"%~dp0runtime\python\pythonw.exe" -m portable_py_ide
if errorlevel 1 (
  echo.
  echo PortablePyIDE failed to start. Please send this window to the teacher.
  pause
)
