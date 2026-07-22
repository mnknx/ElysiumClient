@echo off
REM ElysiumClient - tek tikla Release olustur + oyun paketini yukle
REM Cift tikla: surum sorar (bos = son tag), tag/release olusturur, ZIP paketler ve GitHub'a yukler.
chcp 65001 >nul
cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0release_github.ps1" %1
echo.
pause
