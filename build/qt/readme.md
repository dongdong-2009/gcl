
## 版本文件 ###
版本文件：gcl_version.h、gcl_version.pri
如果没有这两个文件请执行批处理文件
platform_cpp\tools\platform_cpp\dll\gcl_ccxx_standard \create_svn_version_file.bat 
注意：在发布产品前必须执行create_svn_version_file.bat批处理文件，来保证版本号与SVN版本一致。


## SVN ##
注意：注意：注意：
SVN中的https://10.31.16.253/svn/ygct_ics_cpp注意：
大家提交代码时不要提交以下7个 pro 文件：
gcl_qt_projects.pro
dll.pro
main.pro
vxd.pro
app.pro
tools.pro
test.pro
（增加项目除外）
