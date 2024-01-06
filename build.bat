@echo off

if exist cmake-build-debug\ (
    echo Already configured.
) else (
    cmake . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DGCGP_BUILD_TESTS=ON
    if %errorlevel% neq 0 exit /b
)

cmake --build cmake-build-debug --config=Debug
if %errorlevel% neq 0 exit /b
