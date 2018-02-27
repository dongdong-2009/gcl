
@echo off

set sExecDisk=%~d0%
set sExecPath=%~dp0%\
set sExecPath=%sExecPath:\\=\%

echo 在头文件中生成版本号
subwcrev.exe %sExecPath% %sExecPath%\create_svn_version_h.version %sExecPath%\gcl_version.h

echo 在pri文件中生成版本号
subwcrev.exe %sExecPath% %sExecPath%\create_svn_version_pri.version %sExecPath%\gcl_version.pri
