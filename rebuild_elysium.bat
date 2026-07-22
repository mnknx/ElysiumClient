@echo off
setlocal
set "VS=C:\Program Files\Microsoft Visual Studio\18\Community"
set "CMAKE=%VS%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1
cd /d "%~dp0"
"%CMAKE%" --build build --target game-client 2>&1
echo === REBUILD EXIT %errorlevel% ===
