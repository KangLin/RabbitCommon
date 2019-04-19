## Qt项目公共库

================================================

作者：康林（kl222@126.com)

------------------------------------------------

### 功能

本人Qt项目的一些公共库

- [x] 关于对话框
- [x] 在线更新

------------------------------------------------

- [![Windows 编译状态](https://ci.appveyor.com/api/projects/status/cy6jwbbysuj7t1wp/branch/master?svg=true)](https://ci.appveyor.com/project/KangLin/rabbitcommon/branch/master)
- [![Linux 编译状态](https://travis-ci.org/KangLin/RabbitCommon.svg?branch=master)](https://travis-ci.org/KangLin/RabbitCommon)

------------------------------------------------

### 编译
- 建立并进入build目录

        git clone --recursive https://github.com/KangLin/RabbitCommon.git
        cd RabbitCommon
        mkdir build

- 编译
  + 用 qmake 

        cd build
        qmake ../RabbitCommon.pro
        make install

      * 参数
        - BUILD_ABOUT=OFF: 关闭编译关于功能
        - BUILD_UPDATE=OFF: 关闭编译在线更新功能
          
  + 用 cmake
  
        cd build
        cmake .. -DQt5_DIR=${QT_ROOT}/lib/cmake/Qt5
        cmake --build .

      * 参数
        - Qt5_DIR: Qt 位置
        - BUILD_APP: 编译应用程序
        - BUILD_ABOUT: 编译关于功能
        - BUILD_UPDATE: 编译在线更新功能
        
- 安装注意  
如果使用在线更新功能，Qt因为版权原因，没有提供openssl动态库，所以必须自己复制openssl的动态库到安装目录下。
    - 如果是32的，可以在Qt安装程序Tools\QtCreator\bin目录下，找到openssl的动态库（libeay32.dll、ssleay32.dll）
    - 如果是64位，则需要自己下载openssl的二进制安装包。

------------------------------------------------

### 其它应用使用本项目
- 直接用源码
  + QT工程文件
    - 子模块方式：
      + 增加子模块：
      
            git submodule add https://github.com/KangLin/RabbitCommon.git 3th_libs/RabbitCommon
      
      + 在工程文件(.pro)中直接引入 RabbitCommon.pri

            include(3th_libs/RabbitCommon/RabbitCommon.pri)

    - 非子模块方式：在环境变量（RabbitCommon_DIR） 或 QMAKE参数 （RabbitCommon_DIR） 
      中指定 RabbitCommon 源码根目录的位置，然后在主工程文件（.pro）中加入下列：
    
            isEmpty(RabbitCommon_DIR): RabbitCommon_DIR=$$(RabbitCommon_DIR)
            !isEmpty(RabbitCommon_DIR): exists("$${RabbitCommon_DIR}/Src/RabbitCommon.pri"){
                DEFINES += RABBITCOMMON
                include($${RabbitCommon_DIR}/Src/RabbitCommon.pri)
            } else{
                message("1. Please download RabbitCommon source code from https://github.com/KangLin/RabbitCommon ag:")
                message("   git clone https://github.com/KangLin/RabbitCommon.git")
                error("2. Then set value RabbitCommon_DIR to download root dirctory")
            }
    
  + cmake工程
    - 子模块方式
    
            add_subdirectory(3th_libs/RabbitCommon/Src)
        
    - 非子模块方式
      + 引入以 add_subdirectory 本项目录
      
            set(RabbitCommon_DIR $ENV{RabbitCommon_DIR} CACHE PATH "Set RabbitCommon source code root directory.")
            if(EXISTS ${RabbitCommon_DIR}/Src)
                add_subdirectory(${RabbitCommon_DIR}/Src ${CMAKE_BINARY_DIR}/RabbitCommon)
            else()
                message("1. Please download RabbitCommon source code from https://github.com/KangLin/RabbitCommon")
                message("   ag:")
                message("       git clone https://github.com/KangLin/RabbitCommon.git")
                message("2. Then set cmake value or environment variable RabbitCommon_DIR to download root dirctory.")
                message("    ag:")
                message(FATAL_ERROR "       cmake -DRabbitCommon_DIR= ")
            endif()
            
      + 在使用的工程目录CMakeLists.txt
      
            SET(APP_LIBS ${PROJECT_NAME} ${QT_LIBRARIES})
            if(TARGET RabbitCommon)
                target_compile_definitions(${PROJECT_NAME}
                                PRIVATE -DRABBITCOMMON)
                target_include_directories(${PROJECT_NAME}
                                PRIVATE "${RabbitCommon_DIR}/Src"
                                        "${RabbitCommon_DIR}/Src/export")
                set(APP_LIBS ${APP_LIBS} RabbitCommon)
            endif()
            target_link_libraries(${PROJECT_NAME} ${APP_LIBS})

- 以库方式使用使用
  + Qt 工程文件
  + cmake
    cmake 参数 RabbitCommon_DIR 指定安装根目录
    
        find_package(RabbitCommon)

- 加载资源

        CRabbitCommonTools::Instance()->Init();

- [关于对话框](Src/DlgAbout/DlgAbout.h)

  ```
  #ifdef RABBITCOMMON
      CDlgAbout about(this);
      about.m_AppIcon = QPixmap(":/image/Calendar");
      about.m_szHomePage = "https://github.com/KangLin/LunarCalendar";
      about.exec();
  #endif
  ```
  
![关于对话框](docments/image/about.PNG "关于对话框")

- [在线更新功能](Src/FrmUpdater/FrmUpdater.h)

  ```
  #ifdef RABBITCOMMON
      CFrmUpdater *fu = new CFrmUpdater();
      fu->SetTitle(qApp->applicationDisplayName(), QPixmap(":/image/Calendar"));
      fu->show();
  #endif
  ```
  
![在线更新功能](docments/image/update.PNG "在线更新功能")

------------------------------------------------

### 下载
https://github.com/KangLin/RabbitCommon/releases/latest

------------------------------------------------

## 捐赠
- 捐赠(大于￥20)：  
![捐赠( 大于 ￥20 )](https://raw.githubusercontent.com/KangLin/Tasks/master/Src/Resource/image/Contribute.png "捐赠(大于￥20)")

- 捐赠￥20  
![捐赠￥20](https://raw.githubusercontent.com/KangLin/Tasks/master/Src/Resource/image/Contribute20.png "捐赠￥20")

------------------------------------------------

### [许可协议](License.md "License.md")
