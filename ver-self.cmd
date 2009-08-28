: Add version info resource with some strings and a resource file to verpatch.exe
: (a kind of self test)

: run in Release or Debug
:
set _ver="1.0.0.5 [%date%]"
set _s1=/s desc "Version patcher tool" /s copyright "(C) 1998-2009, pavel_a#fastmail.fm"
set _s2=/s pb "pa.test"
set _sf1=/rf #64 ..\usage.txt
set _s2=%_s2% /pv "1.0.0.1 (free)" 
: Run a copy of the exe on itself:
copy verpatch.exe v.exe || exit /b 1
v.exe verpatch.exe /va %_ver% %_s1% %_s2% %_sf1% 
@echo Errorlevel=%errorlevel%
