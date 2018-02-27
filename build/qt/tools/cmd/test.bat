
@echo off

set /a count=2

if %count% equ 2 (
echo "equ"
)

set s="2"

set /a i=1+1+%s:0=%

echo %i%

pause