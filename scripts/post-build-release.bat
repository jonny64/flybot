set PATH=..\tools;%PATH%
cd "..\setup"
SubWCRev.exe ..\ verpatch.src verpatch.bat
set platformName = %1
call verpatch.bat platformName

copy /y "..\Translation\Russian\*.mo" "..\bin"

echo generating packaging script...
echo !define VERSION %VERSION% > flybot.nsi
echo OutFile ..\release\${VERSION}\flybot-${VERSION}.exe >> flybot.nsi
more flybot.src >> flybot.nsi

echo packaging...
if not exist "%PROGRAMFILES%\NSIS\makensis.exe" GOTO NONSIS
"%PROGRAMFILES%\NSIS\makensis" /v2 flybot.nsi

if not exist "..\release\%VERSION%" mkdir "..\release\%VERSION%"
"c:\Program Files\7-Zip\7z" a -tzip "..\release\%VERSION%\Chatbot.%VERSION%.pdb.zip" "..\bin\Chatbot.pdb"

pause
exit

:NONSIS
echo You don't have NSIS installed. Aborting.
exit -2
