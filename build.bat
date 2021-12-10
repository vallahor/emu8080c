@echo off

if not exist build mkdir build

set src=..\src

pushd build
cl /std:c11 /FC /nologo /EHsc /Fe:emu8080c.exe %src%\main.c
popd
