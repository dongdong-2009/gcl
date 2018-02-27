YgctOpcClient.exe -ExecMode show -VFYEnabled true

#-DescribePlugins SettingPlugin=;TerminalPlugin=; -ProjectFile ./describe/iec_104_v100.project.ini -TerminalFile ./config/iec_104_v100.config.ini
-pi SettingPlugin=;TerminalPlugin=; -pf ./describe/iec_104_v100.project.ini -tf ./config/iec_104_v100.config.ini

terminal=config/iec_104_v100.config.ini

#运行模式（显示窗体、隐藏到托盘或后台程序）
-ExecMode [show|hide]
#验证使能
-VFYEnabled [true|false]
#验证类型
-VFYType [user|icard|...]
#验证参数
-VFYParam user=a;pwd=a;icard=xxxx;
#项目文件
-ProjectFile [ProjectXXX.ini]
#描述插件
-DescribePlugins PluginNameXX=PluginParamXX;
#Terminals Config FilePath
#[Terminals] NameTypes="psm_sdk=Custom,psm_sdk=Custom"
-TerminalFile ./config/apps.config.ini

1、ICS本地配置服务 - 本地文件目录管理。
2、ICS本地配置服务 - 文件与目录打包与解压。
3、ICS本地配置服务 - 数据上传与下载。
4、ICS本地配置服务 - 本地程序控制。
5、ICS本地配置服务 - 本地程序状态上报。


1、ICS远程配置工具 - 区域配置管理。
2、ICS远程配置工具 - 前置终端总表配置。
3、ICS远程配置工具 - 前置终端总表生成各终端配置。
4、ICS远程配置工具 - 实时点总表配置。
5、ICS远程配置工具 - 实时点总表生成各终端点表。
6、ICS远程配置工具 - 分管远程文件目录。
7、ICS远程配置工具 - 文件与目录打包与解压。
8、ICS远程配置工具 - 远程数据接收，下装。
9、ICS远程配置工具 - 远程程序控制。
10、ICS本地配置服务 - 本地程序状态上报。
11、ICS本地配置服务 - 用户管理。
12、ICS本地配置服务 - 用户登录与操作日志。
