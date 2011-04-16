@echo off

rem A script to copy all dll from the libs directory to the bin directory

if not exist "bin\SDL.dll" (
dir ..\..\..\libs /A:D /B /S > tempListOfDirs.txt
For /F %%A IN (tempListOfDirs.txt) Do (
If Exist %%A* (
XCOPY %%A\*.dll "bin" /Y /R
)
)
del tempListOfDirs.txt
)