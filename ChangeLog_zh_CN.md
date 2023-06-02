### 版本： v2.0.0
+ 删除 RabbitCommonLog
+ 修改样式接口
+ 自动化 ( github action ) 完成
+ 增加调试日志停泊条
+ Andoird 需要 Qt 大于或等于 6

### 版本： v1.0.13
+ 修复 CFrmUpdater 内存泄漏
+ 文档: 修复 titlebar 注释 BUG

### 版本： v1.0.12
+ 增加　RabbitCommon::CTools::GetLogMenu
+ 修改关于对话框信息

### 版本： v1.0.11
+ 增加版本函数: RabbitCommon::CTools::Version
+ 增加函数：RabbitCommon::CTools::Version
+ 修复 ADD_TARGET 不产生版本配置文件错误
+ 修复包含头文件错误
+ 重命名 CFolderBrowser 到 CDockFolderBrowser
+ 修复关于对话框错误

### 版本： v1.0.10
+ CI：
  - 增加拼写检查
  - 增加代码质量检查
  - 在关于对话框中增加详细信息

### 版本： v1.0.9
+ 修改自启动
+ 增加文件夹浏览器（CBrowseFolder） 和自定义标题栏（CTitleBar）

### 版本： v1.0.8
+ CTools: 增加打开日志文件
+ 支持 Qt6 状态机
+ 增加 qt 输出到文件

### 版本： v1.0.7

+ 样式
  + 增加 CFrmStyle
  + 增加图标主题设置
  + 修改样式保存数据，CStyle 不再是对外接口
+ 修改 CStyle, 增加接口：QString CStyle::GetStyleFile()
+ 修改CDir, 增加 QString CDir::GetDirIcons()
+ 增加从多个 URLS 下载同一个文件
+ 废弃 CStyle，用 CFrmStyle 代替
+ 废弃 LOG_MODEL_DEBUG 等日志输出 , 用 qDebug 等 代替

### 版本： v1.0.6

+ 增加 CEvpAES
+ 增加日志
  - 使用 qDebug 和 QLoggingCategory 
  - 增加 OpenLogConfigureFile()
  - 增加 GetLogFile()
  - 增加 OpenLogFolder()
+ 为 Windows 动态库增加版本后缀
+ 增加 windows core dumper
+ 增加 InstallRequiredSystemLibraries
+ 修复除 0 程序崩溃错误

### 版本： v1.0.5
+ 增加日志
+ 增加样式
+ CMAKE: 增加删除安装目标
+ 增加 QUIWidget ： 用 QDialog 模拟主窗口，可以定制主窗口的标题栏
+ 增加应用服务程序接口
+ 增加最近打开菜单
+ 增加 Log4Qt
+ 增加 doxygen
+ 增加 cmark 支持 MarkDown 语法
+ 增加 cmark_gfm 支持 GitHub MarkDown 语法
+ CMake: 为 android 增加安装目标 INSTALL_APK
+ 增加加密库
+ 支持 Qt6
+ 增加产生更新文件程序
+ 编译增加 WITH_GUI

### 版本： v1.0.4
+ CMake: 增加 add_target 宏
+ 修改 Translations.pri
+ 静态库不分发 Qt dll
+ CMake: 增加插件目标

### 版本： v1.0.3
+ BUG：在LINUX下，修改平台架构

### 版本： v1.0.2
+ 修改 unix 下 appimage 安装更新
+ 增加得到当前用户名接口(getusername)
+ Android: 增加签名
+ FIX：关于对话框大小
+ 修改：在 unix 下库存放到 lib 目录下

### 版本： v1.0.1
+ 增加 data 和 database 目录
+ 修改程序只读目录（android assert)
+ 增加 GetDirDatabaseFile
+ 修改关于和更新对话框中的图标接口

### 版本： v1.0.0
+ 使用命名空间 RabbitCommon
+ 增加开机自启动
+ 增加更新不再提示
+ 增加目录复制功能
+ Android 关于对话框的文件，放到 assets 下
+ Android 翻译资源，放到 assets 下
+ 固定BUG

### 版本： 0.0.3
+ 增加更新时，管理员权限
+ 修改 deb url
+ 增加 CheckRedirectXmlFile

### 版本： 0.0.2
+ 增加ci
+ 修改ANDROID
+ 修改版本比较
+ 修改GlobalDir to RabbitCommonGlobale
+ 修改翻译

### 版本： v0.0.1
+ 关于
+ 更新
