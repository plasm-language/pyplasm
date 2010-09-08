cd "%~dp0"

set PYTHONPATH=.;.\Release
set PATH=%PATH%;".\Release"

c:\python26\python.exe test.py

pause