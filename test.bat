@echo off

call build.bat
if %errorlevel% neq 0 exit /b
ctest --test-dir cmake-build-debug -C Debug %*
