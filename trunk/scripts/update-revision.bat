for /f "tokens=1,2 delims= " %%a in ('svn.exe st -q ..') do (
  if "%%a"=="M" (
    echo update-revision.bat : Deploy error E0001 : "%%b" changed. Commit changes before deploy!
    exit /b 1
  )
)