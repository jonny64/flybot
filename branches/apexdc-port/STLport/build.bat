@echo off
del build\lib\obj /S /Q
rem multithreaded  version
call configure msvc9 --rtl-dynamic
cd build\lib
call "c:\Program Files\Microsoft Visual Studio 9.0\VC\bin\vcvars32.bat"
nmake clean install