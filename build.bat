@echo off
setlocal enabledelayedexpansion

set CLEAN=0
set CONFIGS=
set TARGET=makebuild

:parse_args
if "%~1" == "" goto end_parse
if "%~1" == "-dev" (set CONFIGS=!CONFIGS! Dev)
if "%~1" == "-debug" (set CONFIGS=!CONFIGS! Debug)
if "%~1" == "-release" (set CONFIGS=!CONFIGS! Release)
if "%~1" == "-clean" (set CLEAN=1)
shift
goto parse_args
:end_parse

if "%CONFIGS%" == "" set CONFIGS=Dev

echo Generating build files...
cmake -B build -S . -G "Ninja Multi-Config" --fresh

for %%C in (%CONFIGS%) do (
    if %CLEAN% == 1 (
        echo Cleaning build for %TARGET% (config %%C)
        cmake --build build --config %%C --target clean
    )
    echo Building target %TARGET% with configuration %%C
    cmake --build build --config %%C --target %TARGET%
)
