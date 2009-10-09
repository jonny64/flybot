set PATH=..\tools;%PATH%

set FILEDESCR=/s desc "chatbot for FlylinkDC++ client"
set COMPINFO=/s company "astro64m" /s (c) "(c) copyleft 2008-2009"

set REVISION=
subwcrev .. | gawk "NR==2 { print $5 }" > revision.txt
for /f "usebackq delims=" %%g in ("revision.txt") do set REVISION=%%g
set PRODUCTVERSION=/pv %REVISION%.0

echo updating version information...
verpatch.exe ..\bin\Chatbot.dll %REVISION%.0 %PRODUCTVERSION% %FILEDESCR% %COMPINFO%
echo .. complete