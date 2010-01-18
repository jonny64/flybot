rem setlocal EnableExtensions EnableDelayedExpansion
set PATH=..\tools;%PATH%

set VERSION=
verpatch ..\bin\Chatbot.dll /vo /xi | gawk "/ProductVersion/ { print gensub(/\.0/, \"\", 1, $3) }" > version.txt
for /f "usebackq delims=" %%g in ("version.txt") do set VERSION=%%g
set VERSION=%VERSION:"=%-beta3
