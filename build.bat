@echo off

call "%USERPROFILE%\code\msvc\setup.bat"

if not exist SDL2.dll call sdl.bat
if not exist lib\x64\box2d.lib call box2d.bat

set exe=auto-pong.exe
set libs=Shell32.lib SDL2.lib SDL2main.lib box2d.lib
set cflags=-Zi -nologo -Fe: %exe% /I include
set ldflags=/link /subsystem:console /libpath:lib\x64 %libs%
set source=main.c

cl %cflags% %source% %ldflags%
