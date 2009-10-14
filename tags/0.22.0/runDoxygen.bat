mkdir .\help
mkdir .\help\html
del /S /Q .\help\html\*
"D:\Program Files\doxygen\bin\doxygen.exe" flybot.doxygen
"D:\Program Files\HTML Help Workshop\hhc.exe" .\help\html\index.hhp
pause