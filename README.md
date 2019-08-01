## Qt common library

================================================

Author：Kang Lin（kl222@126.com)

- [![Windows status](https://ci.appveyor.com/api/projects/status/cy6jwbbysuj7t1wp/branch/master?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitcommon/branch/master)
- [![Linux status](https://travis-ci.org/KangLin/RabbitCommon.svg?branch=master)](https://travis-ci.org/KangLin/RabbitCommon)

[<img src="https://raw.githubusercontent.com/KangLin/Tasks/master/Resource/Image/china.png" alt="Chinese" title="Chinese" width="16" height="16" />Chinese](README_zh_CN.md)

### Functions

Qt common library

- [x] About
- [x] Updater
- [x] Tools
  - [x] Execute the program with administrator privileges
  - [x] The program starts automatically
- [x] Cross-platform, support multiple operating systems
  - [x] Windows
  - [x] Linux、Unix
  - [x] Android
  - [ ] Mac os
  - [ ] IOS
  
  Mac os and IOS, I don't have the corresponding equipment,
  please compile and test the students with the corresponding equipment.

### Compile
- Create and entry build directory

        git clone --recursive https://github.com/KangLin/RabbitCommon.git
        cd RabbitCommon
        mkdir build

- Compile
  + Use qmake 

        cd build
        qmake ../RabbitCommon.pro
        make install

      * Parameter
        - BUILD_ABOUT=OFF: Off build about
        - BUILD_UPDATE=OFF: Off build updater
        - BUILD_ADMINAUTHORISER＝OFF: Off admin authoriser
 
  + Use cmake
  
        cd build
        cmake .. -DQt5_DIR=${QT_ROOT}/lib/cmake/Qt5
        cmake --build .

      * Parameter
        - Qt5_DIR: Qt install position
        - BUILD_APP: build app
        - BUILD_ABOUT: build about
        - BUILD_UPDATE: build updater
        - BUILD_ADMINAUTHORISER: build admin authoriser
        
- Compilation notes:
    Use Qtcreate compile for android in windows, It may appear that the dependent library could not be found.

       Don't find D:\Source\build-RabbitCommon-Android_for_armeabi_v7a_Clang_Qt_5_12_4_for_Android_ARMv7-Debug\bin\libRabbitCommon.so
       move libRabbitCommon.so ..\bin\libRabbitCommon.so
       process_begin: CreateProcess(NULL, move libRabbitCommon.so ..\bin\libRabbitCommon.so, ...) failed.
       make (e=2): The system can not find the file specified。
       make[1]: [..\bin\libRabbitCommon.so] Error 2 (ignored)

    The reason is that the make program under windows cannot convert the \ in the path.
    Solution: Use a make program that recognizes the \ in the path, for example: mingw32-make.exe.

- Install notes:  
    + windows   
       If you build app. Qt does not provide openssl dynamic library for copyright reasons, so you must copy the dynamic library of openssl to the installation directory.
        - If it is 32, you can find the dynamic library of openssl (libeay32.dll, ssleay32.dll) in the Qt installer Tools\QtCreator\bin directory.
        - If it is 64-bit, you will need to download the binary installation package for openssl yourself.

    + linux

     ```
     sudo apt-get install libssl1.1
     ```

### Other application use the project
- Direct source code
  + QT pro file
    - submodule：
      + Add submodule：
      
            git submodule add https://github.com/KangLin/RabbitCommon.git 3th_libs/RabbitCommon
      
      + Introduce RabbitCommon.pri directly in the project file (.pro)

            include(3th_libs/RabbitCommon/RabbitCommon.pri)

    - No submodule：Specify the location of the RabbitCommon source root 
      in the environment variable (RabbitCommon_DIR)
      or QMAKE parameter (RabbitCommon_DIR),
      then add the following to the main project file (.pro):
    
            isEmpty(RabbitCommon_DIR): RabbitCommon_DIR=$$(RabbitCommon_DIR)
            !isEmpty(RabbitCommon_DIR): exists("$${RabbitCommon_DIR}/Src/RabbitCommon.pri"){
                DEFINES += RABBITCOMMON
                include($${RabbitCommon_DIR}/Src/RabbitCommon.pri)
            } else{
                message("1. Please download RabbitCommon source code from https://github.com/KangLin/RabbitCommon ag:")
                message("   git clone https://github.com/KangLin/RabbitCommon.git")
                error("2. Then set value RabbitCommon_DIR to download root dirctory")
            }
    
     - Add about files:

            isEmpty(PREFIX) {
                qnx : PREFIX = /tmp
                else : ios: PREFIX=/
                else : android : PREFIX = /
                else : unix : PREFIX = /opt/RabbitCommon
                else : PREFIX = $$OUT_PWD/install
            }

            DISTFILES += Authors.md \
                Authors_zh_CN.md \
                ChangeLog.md \
                License.md

            other.files = $$DISTFILES
            android: other.path = $$PREFIX/assets
            else: other.path = $$PREFIX
            other.CONFIG += directory no_check_exist 
            INSTALLS += other

    Because this way translation resources will be repeated in the target project.
    Therefore, The main pro file is TEMPLATE = subdirs, generally add the RabbitCommon directory under
    the target project source root directory,
    and then link to the project in this directory.
    Can see ：https://github.com/KangLin/Tasks
    
    - Static library

            CONFIG(static): DEFINES *= RABBITCOMMON_STATIC_DEFINE

  + cmake
    - Submodule
    
            add_subdirectory(3th_libs/RabbitCommon/Src)
        
    - No submodule
      + Introduced to add_subdirectory this directory

            if(NOT RabbitCommon_DIR)
                set(RabbitCommon_DIR $ENV{RabbitCommon_DIR})
                if(NOT RabbitCommon_DIR)
                    set(RabbitCommon_DIR ${CMAKE_SOURCE_DIR}/..)
                endif()
            endif()
            if(DEFINED RabbitCommon_DIR AND EXISTS ${RabbitCommon_DIR}/Src)
                add_subdirectory(${RabbitCommon_DIR}/Src ${CMAKE_BINARY_DIR}/RabbitCommon)
            else()
                message("1. Please download RabbitCommon source code from https://github.com/KangLin/RabbitCommon")
                message("   ag:")
                message("       git clone https://github.com/KangLin/RabbitCommon.git")
                message("2. Then set cmake value or environment variable RabbitCommon_DIR to download root dirctory.")
                message("   ag:")
                message(FATAL_ERROR "       cmake -DRabbitCommon_DIR= ")
            endif()

      + CMakeLists.txt in the project used
      
            SET(APP_LIBS ${PROJECT_NAME} ${QT_LIBRARIES})
            if(TARGET RabbitCommon)
                target_compile_definitions(${PROJECT_NAME}
                                PRIVATE -DRABBITCOMMON)
                target_include_directories(${PROJECT_NAME}
                                PRIVATE ${RabbitCommon_DIR}/Src
                                        ${CMAKE_BINARY_DIR})
                set(APP_LIBS ${APP_LIBS} RabbitCommon)
            endif()
            target_link_libraries(${PROJECT_NAME} ${APP_LIBS})

    - Static library

             target_compile_definitions(${PROJECT_NAME} PRIVATE RABBITCOMMON_STATIC_DEFINE)

- Use in library mode
  + Qt pro file
  + cmake
    Cmake parameter RabbitCommon_DIR specifies the installation root directory
    
        find_package(RabbitCommon)

- Load resource

        RabbitCommon::CTools::Instance()->Init();

- [About](Src/DlgAbout/DlgAbout.h)

    + Install Authors、 License、 ChangeLog files.  
      File name naming rules:  
      Authors.md、License.md、ChangeLog.md is the default file.  
      The local file naming rule is to add the local name after the default file name.  
      For example: Chinese file:  
      Authors_zh_CN.md、License_zh_CN.md、ChangeLog_zh_CN.md

    
            isEmpty(PREFIX) {
                qnx : PREFIX = /tmp
                else : ios: PREFIX=/
                else : android : PREFIX = /
                else : unix : PREFIX = /opt/RabbitCommon
                else : PREFIX = $$OUT_PWD/install
            }

            DISTFILES += Authors.md \
                Authors_zh_CN.md \
                ChangeLog.md \
                License.md

            other.files = $$DISTFILES
            android: other.path = $$PREFIX/assets
            else: other.path = $$PREFIX
            other.CONFIG += directory no_check_exist 
            INSTALLS += other
            
    + Used in code
    
            ```
            QApplication a(argc, argv);
            a.setApplicationVersion(BUILD_VERSION);
            a.setApplicationName("Calendar");
            a.setApplicationDisplayName(QObject::tr("Calendar"));
        
            #ifdef RABBITCOMMON
                CDlgAbout about(this);
                about.m_AppIcon = QPixmap(":/image/Calendar");
                about.m_szHomePage = "https://github.com/KangLin/LunarCalendar";
                #if defined (Q_OS_ANDROID)
                    about.showMaximized();
                #endif
                about.exec();
            #endif
            ```
  
![About](docments/image/about_en.png "About")

- [Updater](Src/FrmUpdater/FrmUpdater.h)

  ```
  #ifdef RABBITCOMMON
      CFrmUpdater *fu = new CFrmUpdater();
      fu->SetTitle(qApp->applicationDisplayName(), QPixmap(":/image/Calendar"));
      #if defined (Q_OS_ANDROID)
          fu->showMaximized();
      #else
          fu->show();
      #endif
  #endif
  ```
  
Add Update/update.xml in project source directory

        <?xml version='1.0' encoding='UTF-8'?>
        <REDIRECT>
            <VERSION>0.0.8</VERSION>
        </REDIRECT>
  
![Updater](docments/image/about_en.png "Updater")

- [Execute the program with administrator privileges](Src/AdminAuthoriser/adminauthoriser.h)
    + Internal implementation

            QString szCmd = "mkdir";
            QStringList paras;
            paras << "-p" << "/opt/RabbitCommonAdminAuthoriseTest";
            qDebug() << "RabbitCommon::AdminAuthoriser::Instance()->execute(szCmd, paras):"
                     << RabbitCommon::AdminAuthoriser::Instance()->execute(szCmd, paras);

    + Public interface:

            RabbitCommon::CTools::executeByRoot("regedit", QStringList());

- [The program starts automatically](Src/RabbitCommonTools.h)

        static int InstallStartRun(const QString &szName = QString(),
                               const QString &szPath = QString(),
                               bool bAllUser = false);
        static int RemoveStartRun(const QString &szName = QString(),
                              bool bAllUser = false);
        static bool IsStartRun(const QString &szName = QString(),
                           bool bAllUser = false);

- [Directory function](Src/RabbitCommonDir.h)

## Other projects using this project
- [Tasks](https://github.com/KangLin/Tasks)
- [LunarCalendar](https://github.com/KangLin/LunarCalendar)
- [SerialPortAssistant](https://github.com/KangLin/SerialPortAssistant)

## Donation
- Donation(more than the ￥20)：  
![Donation(more than the ￥20 )](Src/Resource/image/Contribute.png "Donation(more than the ￥20)")

- Donation ￥20  
![Donation ￥20](Src/Resource/image/Contribute20.png "Donation ￥20")

### [License](License.md "License.md")
