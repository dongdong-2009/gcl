
@echo off
set sCmdRoot=%~dp0%\
set sCmdRoot=%sCmdRoot:\\=\%

if exist "%sCmdRoot%cmd.bat" call "%sCmdRoot%cmd.bat"


goto end

:end
