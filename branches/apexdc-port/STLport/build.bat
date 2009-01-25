@echo off
del build\lib\obj /S /Q
cd build\lib
call "c:\Program Files\Microsoft Visual Studio 9.0\VC\bin\vcvars32.bat"
nmake clean install