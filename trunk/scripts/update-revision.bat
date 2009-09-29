call svn update ..
for /f "tokens=1,2 delims= " %%a in ('svn.exe st -q ..') do (
  if "%%a"=="M" (
    echo "%%b" changed
    echo Commit changes before making distrib!
    exit /b 1
  )
)