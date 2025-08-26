### 版本： v2.3.4
  - QTextBrowser 替换 QTextEdit
  - 修改 markdown 到 html
  - 日志：修复 m_szPath 是空错误
  - 增加图标
  - 更新: 增加更新回调函数
  - 用　QTextBrowser 替换 QTextEdit
  - 完成　MacOS 打包

### 版本： v2.3.3
  - 修改在 linux 下用管理员权限启动程序。增加 pkexec
  - 更新：修改 AppImage
  - 移除捐赠比特币
  - RabbitCommonUtils: 修改 RPATH 为 "$ORIGIN:$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
  - CTools
    - 增加 bool CTools::StartWithAdministratorPrivilege(bool bQuitOld)
    - 重命名 executeByRoot 为 ExecuteWithAdministratorPrivilege

### 版本： v2.3.2
  - 修改日志文件名
  - 修改安装目录

### 版本： v2.3.1
  - 修复更新的下载地址

### 版本： v2.3.0
  - 修复日志文件名错误
  - 修复日志析构后，默认处理器错误
  - 增加实时监控日志配置文件，变化后立即更新日志配置
  - 在关于对话框中增加环境变量
  - 增加 CDlgEdit
  - 增加 CFileBrowser
  - 增加文件浏览器应用程序
  - 增加打印调用堆栈
  - 增加 CTools::HasAdministratorPrivilege()
  - CMake: 修改 qt 库为私有

### 版本： v2.2.6
  - 修改 debian 打包文件
  - LINUX下，使用系统安装的 cmark 

### 版本： v2.2.5
  - 修复在 android 上，关于窗口加载文件错误
  - 修改 share/org.Rabbit.RabbitCommon.desktop 的 Categories 值
  - debian/rules: 修改并行数

### 版本： v2.2.4
- RabbitCommonUtils
  - 在 INSTALL_DIR 和 INSTALL_ICON_THEME 增加参数 PARAMETERS
- 重命名图标方案，增加前缀： rabbit-*
- CMake：修改版本函数

### 版本： v2.2.3
- 关于对话框：修复拼写错误
- 修改自动化编译
- 修改 logqt.ini ，加入详细的使用说明
- 修改 RabbitCommonUtils.cmake

### 版本： v2.2.2
- 测试： 增加 CFrmUpdater::CompareVersion 测试用例

### 版本： v2.2.1
- CMAKE：修改安装依赖库
- 移动 translations 到 share/translations

### 版本： v2.2.0
- 增加接口：
  - CTools::InstallTranslator
  - CTools::RemoveTranslator
  - CDir::GetDirDocument
- 修复:
  - INSTALL_LOG_CONFIG_FILE 错误
  - 下载: 修复 file 指针是空错误
- CMake: windeployqt 使用 --compiler-runtime

### 版本： v2.1.0
- 修复 linux 下动态库接错误 #1  
  8 in https://github.com/KangLin/RabbitRemoteControl/issues/20
- CMake: 
  - 非 windows 系统，删除版本是的 v 
  - 修改默认输出目录
    - windows
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    - android
      Use cmake default output directory
    - other
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
- 修改 GetDirData() share from data  
   See: [GNUInstallDirs](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html#module:GNUInstallDirs)
- 修改 ChangeLog.md 等文档存放位置为 ${CMAKE_INSTALL_DOCDIR}
- RabbitCommonUtils: 使用 INSTALL_RPATH  
  修复: 7 in https://github.com/KangLin/RabbitRemoteControl/issues/20
- 修复 CFrmUpdater::CompareVersion 错误
- 修改 CTools::Version()

### 版本： v2.0.3
- 修复footer.html中链接错误

### 版本： v2.0.2
- 修复安装依赖库 libssl 。
  因为 -DX_VCPKG_APPLOCAL_DEPS_INSTALL:BOOL=ON
  安装依赖库。所以需要在程序中引用。
- android 打包时，安装 openssl 库。

### 版本： v2.0.1
- 修复更新中的下载进度条错误
- 关于对话框中使用 CDownLoad 下载捐赠图片

### 版本： v2.0.0
- 删除 RabbitCommonLog
- 自动化 ( github action ) 完成
- 增加调试日志停泊条
- Android 需要 Qt 版本大于或等于 6
- CMake:
  - 增加 INSTALL_DIR
  - 增加 INSTALL_FILE
  - 增加 install dependencies runtime dlls
- 修改样式接口
  - CFrmStyle。改为内部接口
    - 修复 CFrmStyle is deleted on close
    - 增加 RabbitCommon::CTools::AddStyleMenu
    - 增加 RabbitCommon::CTools::InsertStyleMenu
- CDir:
  - 删除 GetOpenDirectory
  - 删除 GetOpenFileName
  - 删除 GetSaveFileName
- 修复自启错误
- RabbitCommon::Tools 只初始化一次
- RabbitCommonUtils: 增加产生 pkg-config 配置文件
- Android:
  - 增加 AndroidRequestPermission
- 修改 debian 打包
- 增加单元测试
- 重命名 RabbitCommon::CDownloadFile 到 RabbitCommon::CDownload
  - 增加 int Start(QVector<QUrl> urls, QString szFileName = QString(), bool bSequence = false);
  - 删除构造函数 CDownloadFile(QVector<QUrl> urls, bool bSequence = false, QObject *parent = nullptr);

### 版本： v1.0.13
- 修复 CFrmUpdater 内存泄漏
- 文档: 修复 titlebar 注释 BUG

### 版本： v1.0.12
- 增加　RabbitCommon::CTools::GetLogMenu
- 修改关于对话框信息

### 版本： v1.0.11
- 增加版本函数: RabbitCommon::CTools::Version
- 增加函数：RabbitCommon::CTools::Version
- 修复 ADD_TARGET 不产生版本配置文件错误
- 修复包含头文件错误
- 重命名 CFolderBrowser 到 CDockFolderBrowser
- 修复关于对话框错误

### 版本： v1.0.10
- CI：
  - 增加拼写检查
  - 增加代码质量检查
  - 在关于对话框中增加详细信息

### 版本： v1.0.9
- 修改自启动
- 增加文件夹浏览器（CBrowseFolder） 和自定义标题栏（CTitleBar）

### 版本： v1.0.8
- CTools: 增加打开日志文件
- 支持 Qt6 状态机
- 增加 qt 输出到文件

### 版本： v1.0.7

- 样式
  - 增加 CFrmStyle
  - 增加图标主题设置
  - 修改样式保存数据，CStyle 不再是对外接口
- 修改 CStyle, 增加接口：QString CStyle::GetStyleFile()
- 修改CDir, 增加 QString CDir::GetDirIcons()
- 增加从多个 URLS 下载同一个文件
- 废弃 CStyle，用 CFrmStyle 代替
- 废弃 LOG_MODEL_DEBUG 等日志输出 , 用 qDebug 等 代替

### 版本： v1.0.6

- 增加 CEvpAES
- 增加日志
  - 使用 qDebug 和 QLoggingCategory 
  - 增加 OpenLogConfigureFile()
  - 增加 GetLogFile()
  - 增加 OpenLogFolder()
- 为 Windows 动态库增加版本后缀
- 增加 windows core dumper
- 增加 InstallRequiredSystemLibraries
- 修复除 0 程序崩溃错误

### 版本： v1.0.5
- 增加日志
- 增加样式
- CMAKE: 增加删除安装目标
- 增加 QUIWidget ： 用 QDialog 模拟主窗口，可以定制主窗口的标题栏
- 增加应用服务程序接口
- 增加最近打开菜单
- 增加 Log4Qt
- 增加 doxygen
- 增加 cmark 支持 MarkDown 语法
- 增加 cmark_gfm 支持 GitHub MarkDown 语法
- CMake: 为 android 增加安装目标 INSTALL_APK
- 增加加密库
- 支持 Qt6
- 增加产生更新文件程序
- 编译增加 WITH_GUI

### 版本： v1.0.4
- CMake: 增加 add_target 宏
- 修改 Translations.pri
- 静态库不分发 Qt dll
- CMake: 增加插件目标

### 版本： v1.0.3
- BUG：在LINUX下，修改平台架构

### 版本： v1.0.2
- 修改 unix 下 appimage 安装更新
- 增加得到当前用户名接口(getusername)
- Android: 增加签名
- FIX：关于对话框大小
- 修改：在 unix 下库存放到 lib 目录下

### 版本： v1.0.1
- 增加 data 和 database 目录
- 修改程序只读目录（android assert)
- 增加 GetDirDatabaseFile
- 修改关于和更新对话框中的图标接口

### 版本： v1.0.0
- 使用命名空间 RabbitCommon
- 增加开机自启动
- 增加更新不再提示
- 增加目录复制功能
- Android 关于对话框的文件，放到 assets 下
- Android 翻译资源，放到 assets 下
- 固定BUG

### 版本： 0.0.3
- 增加更新时，管理员权限
- 修改 deb url
- 增加 CheckRedirectXmlFile

### 版本： 0.0.2
- 增加ci
- 修改ANDROID
- 修改版本比较
- 修改GlobalDir to RabbitCommonGlobale
- 修改翻译

### 版本： v0.0.1
- 关于
- 更新
