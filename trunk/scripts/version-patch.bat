set PATH=..\tools;%PATH%

call get-version-string.bat
set BUILDINFO=/s pb "$WCMODS?MODIFIED:Original$"
set FILEDESCR=/s desc "chatbot for FlylinkDC++ client"
set COMPINFO=/s company "astro64m" /s (c) "(c) copyleft 2008-2009"

echo version string: %VERSION%
echo updating version information...
verpatch.exe /va ..\bin\Chatbot.dll %VERSION%.0 %FILEDESCR% %COMPINFO% %PRODINFO% %BUILDINFO%
echo .. complete