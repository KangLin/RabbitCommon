# Author: Kang Lin <kl222@126.com>

project(RabbitCommon)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_PLATFORM "${CMAKE_SYSTEM_NAME}")

#打开 qt 编译工具
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

if(NOT DEFINED QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
endif()
set(_QT_COMPONENTS Core Network)

if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_C_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_C_STANDARD 11)
endif()

message("CMAKE_CXX_COMPILER_ARCHITECTURE_ID:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
message("CMAKE_SYSTEM_PROCESSOR:${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM_PROCESSOR:${CMAKE_HOST_SYSTEM_PROCESSOR}")

include(CMakeDependentOption)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommonUtils.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Download.cmake)

SET(RabbitCommon_VERSION "v1.0.13")
SET(RABBITCOMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
GET_VERSION(SOURCE_DIR ${RABBITCOMMON_SOURCE_DIR}
    OUT_VERSION RabbitCommon_VERSION
    OUT_REVISION RabbitCommon_REVISION)
message("RabbitCommon_VERSION: ${RabbitCommon_VERSION}")
message("RabbitCommon_REVISION: ${RabbitCommon_REVISION}")
set(VERSION ${RabbitCommon_VERSION})

SET(RABBITCOMMON_INSTALL_HEAD_FILES "")
set(RABBITCOMMON_SOURCE_FILES )
set(RABBITCOMMON_SOURCE_UI_FILES )
set(RABBITCOMMON_HEAD_FILES )
set(RABBITCOMMON_INCLUDE_DIRS
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

option(WITH_GUI "Set ON to with gui" ON)
if(WITH_GUI)
    LIST(APPEND _QT_COMPONENTS Gui Widgets Svg)
    LIST(APPEND RabbitCommon_DEFINITIONS HAVE_RABBITCOMMON_GUI)
    list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES
        RabbitRecentMenu.h
        Style/FrmStyle.h
        TitleBar.h
        DockFolderBrowser/DockFolderBrowser.h
        )
    list(APPEND RABBITCOMMON_HEAD_FILES
        Style/Style.h
        Log/DockDebugLog.h
        Log/DlgFilter.h
        )
    list(APPEND RABBITCOMMON_SOURCE_FILES
        RabbitRecentMenu.cpp
        Style/Style.cpp
        Style/FrmStyle.cpp
        TitleBar.cpp
        DockFolderBrowser/DockFolderBrowser.cpp
        Log/DockDebugLog.cpp
        Log/DlgFilter.cpp
        )
    list(APPEND RABBITCOMMON_SOURCE_UI_FILES
        Style/FrmStyle.ui
        DockFolderBrowser/DockFolderBrowser.ui
        Log/DockDebugLog.ui
        Log/DlgFilter.ui
        )
    list(APPEND RABBITCOMMON_INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Style>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DockFolderBrowser>)
endif()

cmake_dependent_option(BUILD_ABOUT "Set to ON to build about function" ON
    WITH_GUI OFF)
if(BUILD_ABOUT)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_ABOUT)
    list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES DlgAbout/DlgAbout.h)
    list(APPEND RABBITCOMMON_HEAD_FILES DlgAbout/Information.h)
    list(APPEND RABBITCOMMON_SOURCE_FILES DlgAbout/DlgAbout.cpp
        DlgAbout/Information.cpp)
    list(APPEND RABBITCOMMON_SOURCE_UI_FILES DlgAbout/DlgAbout.ui
        DlgAbout/Information.ui)
    
    # 源码目录
    #  Src/DlgAbout
    # 安装目录
    #  ${INSTALL_ROOT}/include/RabbitCommon
    # 要统一使用： 
    #  #include "DlgAbout.h"
    # 所以需要把它加入到 INCLUDE_DIRS
    #  $<BUILD_INTERFACE:...> 只有在编译时才会生成，所以安装时，是空值。
    list(APPEND RABBITCOMMON_INCLUDE_DIRS
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DlgAbout>")
endif(BUILD_ABOUT)

set(CMARK_SHARED OFF CACHE BOOL "Set cmark shared library")
set(CMARK_STATIC ON CACHE BOOL "Set cmark static library")
set(CMARK_TESTS OFF CACHE BOOL "Set cmark tests")
cmake_dependent_option(WITH_CMARK_GFM "Use github cmark. to support Markdown" ON
    BUILD_ABOUT OFF)
if(WITH_CMARK_GFM)

    find_package(cmark-gfm)
    find_package(cmark-gfm-extensions)
    if(NOT cmark-gfm_FOUND)
        SET(_CMARK_GFM_VERSION 0.29.0.gfm.13)
        set(CMARK_GFM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmark/cmark-gfm-${_CMARK_GFM_VERSION}")
        if(NOT EXISTS "${CMARK_GFM_DIR}")
            DOWNLOAD(FILENAME "cmark-gfm-${_CMARK_GFM_VERSION}"
                HASH 65f559c2ad9001cfbac1d6977612c320
                URL "https://github.com/github/cmark-gfm/archive/refs/tags/${_CMARK_GFM_VERSION}.tar.gz"
                DESTINATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmark"
                STATUS retval
                UNPACK
                #SHOW_PROGRESS
                )
            if(NOT retval)
                FILE(REMOVE "${CMARK_GFM_DIR}")
            endif()
            message("Download cmark-gfm:${retval}")
        endif()
        if(EXISTS "${CMARK_GFM_DIR}")
            add_subdirectory("${CMARK_GFM_DIR}"
                ${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm)
            SET(cmark-gfm_FOUND ON)
            set(cmark-gfm-extensions_FOUND ON)
            list(APPEND RABBITCOMMON_PRIVATE_INCLUDE_DIRS
                "$<BUILD_INTERFACE:${CMARK_GFM_DIR}/src>"
                "$<BUILD_INTERFACE:${CMARK_GFM_DIR}/extensions>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm/src>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm/extensions>")
        endif()
    endif(NOT cmark-gfm_FOUND)
    if(cmark-gfm_FOUND AND cmark-gfm-extensions_FOUND)
        list(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_CMARK_GFM)
        if(CMARK_SHARED)
            message(WARNING "Now use -DCMARK_SHARED=ON. suggest to use static library, set cmake parameters: -DCMARK_SHARED=OFF -DCMARK_STATIC=ON")
            list(APPEND RABBIT_PRIVATE_COMMON_LIBS libcmark-gfm-extensions libcmark-gfm)
            if(ANDROID)
                INSTALL_TARGETS(TARGETS libcmark-gfm-extensions libcmark-gfm)
            endif()
        elseif(CMARK_STATIC)
            list(APPEND RABBIT_PRIVATE_COMMON_LIBS libcmark-gfm-extensions_static libcmark-gfm_static)
        elseif(TARGET libcmark-gfm)
            list(APPEND RABBIT_PRIVATE_COMMON_LIBS libcmark-gfm-extensions libcmark-gfm)
            if(ANDROID)
                INSTALL_TARGETS(TARGETS libcmark-gfm-extensions libcmark-gfm)
            endif()
        endif()
    endif()
    
endif(WITH_CMARK_GFM)

cmake_dependent_option(WITH_CMARK "Use cmark. to support Markdown" ON
    "BUILD_ABOUT;NOT WITH_CMARK_GFM" OFF)
if(WITH_CMARK)

    find_package(cmark)
    if(NOT cmark_FOUND)
        #message("Suggest use cmark: https://github.com/commonmark/cmark to support Markdown. will download cmark ...")
        
        SET(_CMARK_VERSION 0.30.2)
        set(CMARK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmark/cmark-${_CMARK_VERSION})
        if(NOT EXISTS "${CMARK_DIR}")
            DOWNLOAD(FILENAME "cmark-${_CMARK_VERSION}"
                HASH d3f527fe8451dd9b58974b52c27302a8
                URL "https://github.com/commonmark/cmark/archive/refs/tags/${_CMARK_VERSION}.tar.gz"
                DESTINATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmark"
                STATUS retval
                UNPACK
                #SHOW_PROGRESS
                )
            if(NOT retval)
                FILE(REMOVE "${CMARK_DIR}")
            endif()
            message("Download cmark:${retval}")
        endif()
        if(EXISTS "${CMARK_DIR}")
            add_subdirectory("${CMARK_DIR}"
                ${CMAKE_CURRENT_BINARY_DIR}/cmark)
            SET(cmark_FOUND TRUE)
        endif()
        
    endif()
    if(cmark_FOUND)
        list(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_CMARK)
        if(CMARK_SHARED)
            message(WARNING "Now use -DCMARK_SHARED=ON. suggest to use static library, set cmake parameters: -DCMARK_SHARED=OFF -DCMARK_STATIC=ON")
            list(APPEND RABBIT_PRIVATE_COMMON_LIBS cmark::cmark)
            if(ANDROID)
                INSTALL_TARGETS(TARGETS cmark::cmark)
            endif()
        elseif(CMARK_STATIC)
            list(APPEND RABBIT_PRIVATE_COMMON_LIBS cmark::cmark_static)
        elseif(TARGET cmark::cmark)
            list(APPEND RABBIT_PRIVATE_COMMON_LIBS cmark::cmark)
            if(ANDROID)
                INSTALL_TARGETS(TARGETS cmark::cmark)
            endif()
        endif()
    endif()
endif(WITH_CMARK)

if(cmark_FOUND OR cmark-gfm_FOUND)
    find_package(Qt${QT_VERSION_MAJOR}WebEngineWidgets)
    if(Qt${QT_VERSION_MAJOR}WebEngineWidgets_FOUND)
        list(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_WebEngineWidgets)
        LIST(APPEND _QT_COMPONENTS WebEngineWidgets)
    else()
        message("Don't find Qt${QT_VERSION_MAJOR}WebEngineWidgets")
        find_package(Qt${QT_VERSION_MAJOR}WebView)
        if(Qt${QT_VERSION_MAJOR}WebView_FOUND)
            list(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_WebView)
            LIST(APPEND _QT_COMPONENTS WebView)
        endif()
    endif()
endif()

cmake_dependent_option(BUILD_UPDATE "Set to ON to build update function" ON
    WITH_GUI OFF)
if(BUILD_UPDATE)
    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        find_package(Qt${QT_VERSION_MAJOR}StateMachine)
        if(Qt${QT_VERSION_MAJOR}StateMachine_FOUND)
            LIST(APPEND _QT_COMPONENTS StateMachine)
        endif()
    else()
        set(Qt${QT_VERSION_MAJOR}StateMachine_FOUND ON)
    endif()
    if(Qt${QT_VERSION_MAJOR}StateMachine_FOUND)
        LIST(APPEND _QT_COMPONENTS Xml)
        list(APPEND RabbitCommon_DEFINITIONS HAVE_UPDATE)
        list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES FrmUpdater/FrmUpdater.h)
        list(APPEND RABBITCOMMON_SOURCE_FILES FrmUpdater/FrmUpdater.cpp)
        list(APPEND RABBITCOMMON_SOURCE_UI_FILES FrmUpdater/FrmUpdater.ui)
        list(APPEND RABBITCOMMON_INCLUDE_DIRS
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FrmUpdater>")
    endif()
endif()

cmake_dependent_option(BUILD_QUIWidget "Set to ON to build QUIWidget" OFF
    WITH_GUI OFF)
if(BUILD_QUIWidget)
    list(APPEND RabbitCommon_DEFINITIONS BUILD_QUIWidget)
    list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES QUIWidget/QUIWidget.h)
    list(APPEND RABBITCOMMON_SOURCE_FILES QUIWidget/QUIWidget.cpp)
    #设置资源文件
    LIST(APPEND RCC_FILES QUIWidget/Resource/QUIWidget.qrc)
    LIST(APPEND RCC_FILES $<$<CONFIG:Debug>:QUIWidget/Resource/QUIWidgetQss.qrc>)
    list(APPEND RABBITCOMMON_INCLUDE_DIRS
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/QUIWidget>")
endif(BUILD_QUIWidget)

find_package(OpenSSL)
IF(OpenSSL_FOUND)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_OPENSSL)
    list(APPEND RABBITCOMMON_SOURCE_FILES EvpAES.cpp)
    list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES EvpAES.h)
    # QtCreator supports the following variables for Android, which are identical to qmake Android variables.
    # Check https://doc.qt.io/qt/deployment-android.html for more information.
    # They need to be set before the find_package( ...) calls below.
    if(ANDROID)
        #set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")

        list(APPEND QT_ANDROID_EXTRA_LIBS
            $<TARGET_FILE:OpenSSL::SSL>
            $<TARGET_FILE:OpenSSL::Crypto>)

    endif()
    list(APPEND RABBIT_PRIVATE_COMMON_LIBS OpenSSL::SSL OpenSSL::Crypto)

    #if(WIN32)
        # Qt 5.9 (LTS) use OpenSSL 1.0* and Qt 5.12 (LTS) and newest version of Qt use OpenSSL 1.1*
        # Source: https://lists.qt-project.org/pipermail/releasing/2019-March/002614.html
    #    INSTALL_TARGETS(TARGETS OpenSSL::SSL OpenSSL::Crypto)
    #endif()

ENDIF()

option(BUILD_ADMINAUTHORISER "Set to ON to build admin authoriser function()" ON)
if(BUILD_ADMINAUTHORISER)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_ADMINAUTHORISER)
    list(APPEND RABBITCOMMON_SOURCE_FILES AdminAuthoriser/adminauthoriser.cpp)
    list(APPEND RABBITCOMMON_HEAD_FILES AdminAuthoriser/adminauthorization_p.h
        AdminAuthoriser/adminauthoriser.h)
    if(WIN32)
        list(APPEND RABBITCOMMON_SOURCE_FILES AdminAuthoriser/adminauthorization_win.cpp)
    elseif(MAC)
        list(APPEND RABBITCOMMON_SOURCE_FILES AdminAuthoriser/adminauthorization_mac.cpp)
    elseif(NOT ANDROID AND UINX)
        list(APPEND RABBITCOMMON_SOURCE_FILES AdminAuthoriser/adminauthorization_x11.cpp)
    else()
        list(APPEND RABBITCOMMON_SOURCE_FILES AdminAuthoriser/adminauthorization_dummy.cpp)
    endif()
endif(BUILD_ADMINAUTHORISER)

OPTION(INSTALL_LOG_CONFIG_FILE "Install configure files" OFF)
if(INSTALL_LOG_CONFIG_FILE)
    if(ANDROID)
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/logqt.ini
            DESTINATION assets/etc
            COMPONENT Runtime)
    else()
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/logqt.ini
            DESTINATION etc
            COMPONENT Runtime)
    endif()
endif()
    
if(WIN32)
    list(APPEND RABBITCOMMON_HEAD_FILES CoreDump/MiniDumper.h)
    list(APPEND RABBITCOMMON_SOURCE_FILES CoreDump/MiniDumper.cpp
        CoreDump/QMiniDumper.cpp)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS
        _UNICODE UNICODE STRSAFE_NO_DEPRECATE)
    list(APPEND RABBIT_PRIVATE_COMMON_LIBS Dbghelp)
endif()

list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES
    RabbitCommonTools.h
    RabbitCommonDir.h
    RabbitCommonEncrypt.h
    Download.h
    )
list(APPEND RABBITCOMMON_HEAD_FILES
    ${RABBITCOMMON_INSTALL_HEAD_FILES}
    RabbitCommonRegister.h
    Log/Log.h
    )
list(APPEND RABBITCOMMON_SOURCE_FILES
    Log/Log.cpp
    RabbitCommonDir.cpp
    RabbitCommonTools.cpp
    RabbitCommonRegister.cpp
    RabbitCommonEncrypt.cpp
    Download.cpp
    )

#翻译
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Translations.cmake)

#设置资源文件
LIST(APPEND RCC_FILES
    Resource/ResourceRabbitCommon.qrc
    )

if(RabbitCommon_VERSION)
    list(APPEND RabbitCommon_DEFINITIONS
        RabbitCommon_VERSION="${RabbitCommon_VERSION}"
        RabbitCommon_REVISION="${RabbitCommon_REVISION}")
else()
    message(WARNING "Please set RabbitCommon_VERSION")
endif()

if(MINGW)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS MINGW)
endif()
if(WIN32)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS WINDOWS UNICODE)
    list(APPEND RABBIT_PRIVATE_COMMON_LIBS Advapi32 Ole32 Shell32 netapi32 Ws2_32)
elseif(UNIX)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS UNIX)
endif()

list(APPEND RabbitCommon_PRIVATE_DEFINITIONS
    BUILD_PLATFORM="${BUILD_PLATFORM}")

if(MSVC)
    set(PRIVATE_OPTIONS "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
        "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)

if(BUILD_TEST)
    set(BUILD_DEFINITIONS HAVE_TEST)
    list(APPEND _QT_COMPONENTS Test)
endif()

#需要的QT组件
if(QT_VERSION VERSION_LESS 6.0.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 6.0.0 for android")
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${_QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
FOREACH(_COMPONENT ${_QT_COMPONENTS})
    if(Qt${QT_VERSION_MAJOR}${_COMPONENT}_FOUND)
        LIST(APPEND RABBIT_COMMON_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
        set(RabbitCommon_QT_COMPONENTS "${RabbitCommon_QT_COMPONENTS} ${_COMPONENT}")
    endif()
ENDFOREACH()
if(${QT_VERSION} VERSION_GREATER 6.0.0 AND ANDROID)
    LIST(APPEND RABBIT_PRIVATE_COMMON_LIBS Qt${QT_VERSION_MAJOR}::CorePrivate)
endif()
get_filename_component(QT_INSTALL_DIR "${Qt${QT_VERSION_MAJOR}_DIR}/../../.." ABSOLUTE)
message("QT_INSTALL_DIR:${QT_INSTALL_DIR}")
message("RabbitCommon_QT_COMPONENTS:${RabbitCommon_QT_COMPONENTS}")

# 设置 Release 时，qDebug 打印文件名与行号
option(QT_MESSAGELOGCONTEXT "Use qt message log context(__FILE__, __LIEN__, __FUNCTION__) in release" OFF)
if(QT_MESSAGELOGCONTEXT)
    list(APPEND RabbitCommon_DEFINITIONS QT_MESSAGELOGCONTEXT)
endif()

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${RABBITCOMMON_SOURCE_FILES} ${RABBITCOMMON_SOURCE_UI_FILES} ${RCC_FILES} ${RABBITCOMMON_HEAD_FILES}
    INSTALL_HEADER_FILES ${RABBITCOMMON_INSTALL_HEAD_FILES}
    INCLUDE_DIRS ${RABBITCOMMON_INCLUDE_DIRS}
    PRIVATE_INCLUDE_DIRS ${RABBITCOMMON_PRIVATE_INCLUDE_DIRS}
    DEFINITIONS ${RabbitCommon_DEFINITIONS}
        $<$<CONFIG:Debug>:DEBUG _DEBUG>
        $<BUILD_INTERFACE: ${BUILD_DEFINITIONS}>
    PRIVATE_DEFINITIONS ${RabbitCommon_PRIVATE_DEFINITIONS}
    LIBS ${RABBIT_COMMON_LIBS}
    PRIVATE_LIBS ${RABBIT_PRIVATE_COMMON_LIBS}
    PRIVATE_OPTIONS ${PRIVATE_OPTIONS}
    FEATURES cxx_std_11
    VERSION ${RabbitCommon_VERSION}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_CMAKE_CONFIG_IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommonConfig.cmake.in
    )

IF(OpenSSL_FOUND)
    # QtCreator supports the following variables for Android, which are identical to qmake Android variables.
    # Check https://doc.qt.io/qt/deployment-android.html for more information.
    # They need to be set before the find_package( ...) calls below.
    if(ANDROID)
        set_property(TARGET ${PROJECT_NAME}
            APPEND PROPERTY QT_ANDROID_EXTRA_LIBS
            $<TARGET_FILE:OpenSSL::SSL>
            $<TARGET_FILE:OpenSSL::Crypto>)
    endif()
ENDIF()

#foreach(d ${RabbitCommon_DEFINITIONS})
#    SET(RabbitCommon_DEFINITIONS_PC "${RabbitCommon_DEFINITIONS_PC} -D${d}")
#endforeach()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommonUtils.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Translations.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommon.pc.in
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RabbitCommon
        COMPONENT Development
    )
# Install pc files
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommon.pc.in
#    ${CMAKE_CURRENT_BINARY_DIR}/RabbitCommon.pc @ONLY)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/RabbitCommon.pc
#    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
#    COMPONENT Development)

## Install style files
INSTALL_STYLE(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/style)
#INSTALL_DIR(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/QUIWidget/Resource/qss
#    DESTINATION data)
# Install QIcon theme
INSTALL_ICON_THEME(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons)
