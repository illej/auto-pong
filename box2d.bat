@echo off

set clean=""

:parse
    if "%~1" == "" (
        goto endparse
    ) else if "%~1" == "/c" (
        set clean=1
    ) else (
        echo Unknown option: %1
        echo Usage: %0 [/c]
        exit /b 1
    )
    shift
    goto parse
:endparse

if "%clean%" == "1" (
    rmdir /S /Q lib
    rmdir /S /Q include
    rmdir /S /Q box2d
)

if not exist lib mkdir lib
if not exist include mkdir include
if not exist box2d call git clone https://github.com/erincatto/box2d.git

cd box2d

rem compile box2d.lib
cl /nologo /c /std:c17 /experimental:c11atomics /Fe:box2d src\*.c /I include
lib /nologo /out:box2d.lib *.obj

copy box2d.lib ..\lib\x64
robocopy include\box2d ..\include\box2d > nul

cd ..

rem compile app
rem cl /nologo main.c /I include /link /libpath:lib box2d.lib
