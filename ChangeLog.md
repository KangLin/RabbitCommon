### Version: v2.0.1
+ Update: fix show progressBar

### Version: v2.0.0
+ Remove RabbitCommonLog
+ CI ( github action ) is ok.
+ Add dock debug log
+ Qt version of Android greater or equal 6
+ CMake:
  - Add INSTALL_DIR
  - Add INSTALL_FILE
  - Add install dependencies runtime dlls
+ Modify style interface
  - CFrmStyle. modify to internal api.
    + Fix CFrmStyle is deleted on close
    + Add RabbitCommon::CTools::AddStyleMenu
    + Add RabbitCommon::CTools::InsertStyleMenu
+ CDir:
  - Remove GetOpenDirectory
  - Remove GetOpenFileName
  - Remove GetSaveFileName
+ Fix auto start
+ Only initialized once RabbitCommon::Tools
+ RabbitCommonUtils: add pkg-config file
+ Android:
  - Add AndroidRequestPermission
+ Modify debian package
+ Add unit test
+ Renmae RabbitCommon::CDownloadFile to RabbitCommon::CDownload
  - Delete the constructor CDownloadFile(QVector<QUrl> urls, bool bSequence = false, QObject *parent = nullptr);
  - Add int Start(QVector<QUrl> urls, QString szFileName = QString(), bool bSequence = false);

### Version: v1.0.13
+ Fix CFrmUpdater memory leak
+ Documents: fix titlebar comments bug

### Version: v1.0.12
+ Add RabbitCommon::CTools::GetLogMenu
+ Modify about Information

### Version: v1.0.11

+ Add RabbitCommon::CTools::Version
+ Add RabbitCommon::CTools::Information
+ Fix ADD_TARGET is not cmake version configure file
+ Fix include directory bug
+ Rename CFolderBrowser to CDockFolderBrowser
+ Fix CDlgAbout bug

### Version: v1.0.10

+ CI
  - Add code spell
  - Add code QL analysis
  - Add details in CDlgAbout

### Version: v1.0.9

+ Add CBrowseFolder and CTitleBar
+ Add CPack

### Version: v1.0.8

+ CTools: add open log file
+ Support Qt6 statemachine
+ Add qt log file
+ Modify start run

### Version： v1.0.7

+ Style
  + Add CFrmStyle
  + Add Icon theme
  + Modify style save data, CStyle is not interface.
+ Modify CStyle, add interface: QString CStyle::GetStyleFile()
+ Modify CDir, add interface: QString CDir::GetDirIcons()
+ Add download the same file from multiple URLs
+ Deprecated CStyle, use CFrmStyle instead
+ Deprecated LOG_MODEL_DEBUG etc log functions, use qDebug etc instead

### Version: v1.0.6

+ Add CEvpAES class
+ Log
  - Use qDebug and QLoggingCategory
  - Add OpenLogConfigureFile()
  - Add GetLogFile()
  - Add OpenLogFolder()
+ Add version suffix to Windows dynamic library
+ Add windows core dumper
+ CMAKE: add InstallRequiredSystemLibraries
+ Fix divide by 0 program crash bug

### Version: v1.0.5
+ Add logger
+ Add style
+ CMake: Add uninstall
+ Add QUIWidget
+ Add service interface
+ Recent open menu
+ Add Log4Qt
+ Add doxygen
+ Add cmark in order to support MarkDown
+ Add cmark-gfm in order to support Github MarkDown
+ CMake: add INSTALL_APK target for android
+ Add encrypt
+ Add support Qt6
+ Add generate update file application
+ Add compile WITH_GUI

### Version: v1.0.4
+ CMake: Add plugin target marco
+ CMake: Add add_target marco
+ Modify Translations.pri
+ FIX: don't deploy qt dll when install static library

### Version: v1.0.3
+ BUG： Modify arch in unix

### Version: v1.0.2
+ Modify updater for appimage in unix
+ Add getusername interface
+ Android: Add sign
+ Fix: about widge size
+ Modify the library directory in UNIX to the library

### Version: v1.0.1
+ Modify application directory reade only
+ Add data and database directory
+ Add GetDirDatabaseFile
+ Modify about and update icon user interface

### Version: v1.0.0
+ Use namespace RabbitCommon
+ Add auto start in boot
+ Add don't prompt in updater
+ Add copy directory
+ Android about files install to assets 
+ Android resource file install to assets 
+ FIX BUG

### Version: 0.0.3
+ Add AdminAuthoriser
+ Modify deb url
+ Add CheckRedirectXmlFile

### Version: 0.0.2
+ Add ci
+ Modify ANDROID
+ Modify version compare
+ Modify GlobalDir to RabbitCommonGlobale
+ Modify translation

### Version: v0.0.1
+ About
+ Updater
