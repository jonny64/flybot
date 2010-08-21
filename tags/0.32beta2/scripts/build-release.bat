cd ..\
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
devenv "flybot.sln" /rebuild "Release|x64"
devenv "flybot.sln" /rebuild "Release|Win32"
pause