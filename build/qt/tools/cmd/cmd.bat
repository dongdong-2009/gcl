
@echo off

rem set sDeployType="debug"
rem set sDeployType="release"
set sDeployType=%1
echo the sDeployType is %sDeployType%

set sCurrentPath=%~dp0%\
set sCurrentPath=%sCurrentPath:\\=\%

set sDeployPath=%sCurrentPath%..\..\..\deploy\win32\ics_svr\gcl_svr\
set sTestPath=%sCurrentPath%..\..\test\


rem 复制拷贝 gcl_vdi1.dll 到 bin_qt bin_qt_d 的目录
set sCopyFile=gcl_vdi1.dll
call :copyToBin

rem 复制拷贝 gcl_script1.dll 到 bin_qt bin_qt_d 的目录
set sCopyFile=gcl_script1.dll
call :copyToBin

rem 复制拷贝 gcl_script_vxd1.dll 到 bin_qt bin_qt_d 的目录
set sCopyFile=gcl_script_vxd1.dll
call :copyToBin

rem 复制拷贝 gcl_sdk1.dll 到 bin_qt bin_qt_d 的目录
set sCopyFile=gcl_sdk1.dll
call :copyToBin

rem 复制拷贝 gcl_database_sqlite1.dll 到 bin_qt bin_qt_d 的目录
set sCopyFile=gcl_database_sqlite1.dll
call :copyToBin

rem 复制拷贝 gcl_database_odbc1.dll 到 bin_qt bin_qt_d 的目录
set sCopyFile=gcl_database_odbc1.dll
call :copyToBin


rem 复制拷贝 gcl_ccxx1.dll, gcl_vdi1.dll, gcl_sdk1.dll, gcl_ga1.dll 到 %sTargetPath% 的目录
set sTargetPath=%sTestPath%sdk_delphi
if %sDeployType%==cmd_debug.bat (
call :copyToSdkD
) else (
call :copyToSdkR
)

set sTargetPath=%sTestPath%bin
if %sDeployType%==cmd_debug.bat (
call :copyToSdkD
) else (
call :copyToSdkR
)

if %sDeployType%==cmd_debug.bat (
set sTargetPath=%sTestPath%sdk_csharp\bin\Debug
call :copyToSdkD
) else (
set sTargetPath=%sTestPath%sdk_csharp\bin\Release
call :copyToSdkR
)


rem 复制拷贝 gcl_svr_script_vxd.exe 到 atm330_ticket.exe
set sTargetFileName=atm330_ticket.exe
call :copy_gcl_svr_script_vxdToNew

rem 复制拷贝 gcl_svr_script_vxd.exe 到 atm540_ticket.exe
set sTargetFileName=atm540_ticket.exe
call :copy_gcl_svr_script_vxdToNew

rem 复制拷贝 gcl_svr_script_vxd.exe 到 atm550_ticket.exe
set sTargetFileName=atm550_ticket.exe
call :copy_gcl_svr_script_vxdToNew

rem 复制拷贝 gcl_svr_script_vxd.exe 到 gcl_data_svr.exe
set sTargetFileName=gcl_data_svr.exe
call :copy_gcl_svr_script_vxdToNew


goto end


:copyToBin

rem gcl_vdi1.dll
if exist "%sDeployPath%bin\%sCopyFile%" (
  echo %time% %sCopyFile% to %sDeployPath%bin_qt
  copy %sDeployPath%bin\%sCopyFile% %sDeployPath%bin_qt\%sCopyFile%
)
if exist "%sDeployPath%bin_d\%sCopyFile%" (
echo %time% %sCopyFile% to %sDeployPath%bin_qt_d
copy %sDeployPath%bin_d\%sCopyFile% %sDeployPath%bin_qt_d\%sCopyFile%
)

goto end


:copyToSdkD

set sCopyFile=gcl_ccxx1.dll
if exist "%sDeployPath%bin_d\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin_d\%sCopyFile% %sTargetPath%\%sCopyFile%
)
set sCopyFile=gcl_vdi1.dll
if exist "%sDeployPath%bin_d\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin_d\%sCopyFile% %sTargetPath%\%sCopyFile%
)
set sCopyFile=gcl_sdk1.dll
if exist "%sDeployPath%bin_d\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin_d\%sCopyFile% %sTargetPath%\%sCopyFile%
)
set sCopyFile=gcl_ga1.dll
if exist "%sDeployPath%bin_d\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin_d\%sCopyFile% %sTargetPath%\%sCopyFile%
)
goto end


:copyToSdkR

set sCopyFile=gcl_ccxx1.dll
if exist "%sDeployPath%bin\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin\%sCopyFile% %sTargetPath%\%sCopyFile%
)
set sCopyFile=gcl_vdi1.dll
if exist "%sDeployPath%bin\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin\%sCopyFile% %sTargetPath%\%sCopyFile%
)
set sCopyFile=gcl_sdk1.dll
if exist "%sDeployPath%bin\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin\%sCopyFile% %sTargetPath%\%sCopyFile%
)
set sCopyFile=gcl_ga1.dll
if exist "%sDeployPath%bin\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetPath%
copy %sDeployPath%bin\%sCopyFile% %sTargetPath%\%sCopyFile%
)

goto end


:copy_gcl_svr_script_vxdToNew

set sCopyFile=gcl_svr_script_vxd.exe
if exist "%sDeployPath%bin\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetFileName%
copy %sDeployPath%bin\%sCopyFile% %sDeployPath%bin\%sTargetFileName%
)
if exist "%sDeployPath%bin_d\%sCopyFile%" (
echo %time% %sCopyFile% to %sTargetFileName%
copy %sDeployPath%bin_d\%sCopyFile% %sDeployPath%bin_d\%sTargetFileName%
)

goto end



:end
