# Author: Kang Lin <kl222@126.com>

project(RabbitCommon)

set(BUILD_PLATFORM "${CMAKE_SYSTEM_NAME}")

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} RABBITCOMMON_BUILD_TYPE)
if(RABBITCOMMON_BUILD_TYPE STREQUAL "debug")
    list(APPEND RabbitCommon_DEFINITIONS DEBUG _DEBUG)
endif()

set(_QT_COMPONENTS Core)

# ----------------------------------------------------------------------------
# Detect compiler and target platform architecture
# ----------------------------------------------------------------------------
if(NOT ANDROID)
    if(X86_64 OR CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(BUILD_ARCH x86_64)
    elseif(X86 OR CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(BUILD_ARCH x86)
    endif()
else()
    set(BUILD_ARCH ${ANDROID_ARCH})
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i386")
  message(STATUS "i386 architecture detected")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i686")
  message(STATUS "i686 architecture detected")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
  message(STATUS "x86_64 architecture detected")
else()
  message(STATUS "host processor architecture is ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

#打开 qt 编译工具
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommonUtils.cmake)

SET(RABBITCOMMON_INSTALL_HEAD_FILES "")
set(RABBITCOMMON_SOURCE_FILES )
set(RABBITCOMMON_SOURCE_UI_FILES )
set(RABBITCOMMON_HEAD_FILES )
option(BUILD_ABOUT "Set to ON to build about function" ON)
if(BUILD_ABOUT)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_ABOUT)
    list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES DlgAbout/DlgAbout.h)
    list(APPEND RABBITCOMMON_SOURCE_FILES DlgAbout/DlgAbout.cpp)
    list(APPEND RABBITCOMMON_SOURCE_UI_FILES DlgAbout/DlgAbout.ui)
endif(BUILD_ABOUT)

option(BUILD_UPDATE "Set to ON to build update function" ON)
if(BUILD_UPDATE)
    LIST(APPEND _QT_COMPONENTS Network Xml)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_UPDATE)
    list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES FrmUpdater/FrmUpdater.h)
    list(APPEND RABBITCOMMON_SOURCE_FILES FrmUpdater/FrmUpdater.cpp)
    list(APPEND RABBITCOMMON_SOURCE_UI_FILES FrmUpdater/FrmUpdater.ui)
endif(BUILD_UPDATE)

if(ANDROID)
    option(BUILD_QUIWidget "Set to ON to build QUIWidget" OFF)
else()
    option(BUILD_QUIWidget "Set to ON to build QUIWidget" OFF)
endif()
if(BUILD_QUIWidget)
    list(APPEND RabbitCommon_DEFINITIONS BUILD_QUIWidget)
    list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES QUIWidget/QUIWidget.h)
    list(APPEND RABBITCOMMON_SOURCE_FILES QUIWidget/QUIWidget.cpp)
    #设置资源文件
    LIST(APPEND RCC_FILES QUIWidget/Resource/QUIWidget.qrc)
    if("debug" STREQUAL BUILD_TYPE)
        LIST(APPEND RCC_FILES QUIWidget/Resource/QUIWidgetQss.qrc)
    endif()
endif(BUILD_QUIWidget)

find_package(OpenSSL)
IF(OpenSSL_FOUND)
    # QtCreator supports the following variables for Android, which are identical to qmake Android variables.
    # Check https://doc.qt.io/qt/deployment-android.html for more information.
    # They need to be set before the find_package( ...) calls below.
    if(ANDROID)
        #set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
        if (ANDROID_ABI STREQUAL "armeabi-v7a")
            set(ANDROID_EXTRA_LIBS
                $<TARGET_FILE:OpenSSL::SSL>
                $<TARGET_FILE:OpenSSL::Crypto>)
        endif()
    else()
        list(APPEND RABBIT_PRIVATE_COMMON_LIBS OpenSSL::SSL OpenSSL::Crypto)
    endif()
    if(WIN32)
        INSTALL_TARGETS(TARGETS OpenSSL::SSL OpenSSL::Crypto)
    endif()
ENDIF()

option(BUILD_ADMINAUTHORISER "Set to ON to build admin authoriser function()" ON)
if(BUILD_ADMINAUTHORISER)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_ADMINAUTHORISER)
    list(APPEND RABBITCOMMON_HEAD_FILES AdminAuthoriser/adminauthoriser)
    list(APPEND RABBITCOMMON_SOURCE_FILES AdminAuthoriser/adminauthoriser.cpp)
    list(APPEND RABBITCOMMON_HEAD_FILES AdminAuthoriser/adminauthorization_p.h)
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

find_package(Log4Qt)
if(Log4Qt_FOUND)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    LIST(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_LOG4QT)
    LIST(APPEND RABBIT_PRIVATE_COMMON_LIBS Log4Qt::log4qt)
    LIST(APPEND _QT_COMPONENTS Concurrent)
    INSTALL_TARGETS(TARGETS "Log4Qt::log4qt")
    # Install log4qt.properties configure file
    if(ANDROID)
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/log4qt.conf
            DESTINATION assets/etc
                COMPONENT Runtime)
    else()
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/log4qt.conf
            DESTINATION etc
                COMPONENT Runtime)
    endif()
else()
    message("Download Log4Qt from https://github.com/MEONMedical/Log4Qt")
endif()

find_package(log4cxx)
if(log4cxx_FOUND)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    LIST(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_LOG4CXX)
    LIST(APPEND RABBIT_PRIVATE_COMMON_LIBS log4cxx)
    # Install log4cplus configure file
    if(ANDROID)
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/log4cxx.conf
            DESTINATION assets/etc
                COMPONENT Runtime)
    else()
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/log4cxx.conf
            DESTINATION etc
                COMPONENT Runtime)
    endif()
else()
    message("Download log4cxx http://gitbox.apache.org/repos/asf/logging-log4cxx.git")
endif()

find_package(log4cplus)
message("log4cplus:${log4cplus_FOUND}")
if(log4cplus_FOUND)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    #message("Found log4cplugs componts:${log4cplus_FIND_COMPONENTS}")
    LIST(APPEND RabbitCommon_PRIVATE_DEFINITIONS HAVE_LOG4CPLUS)
    LIST(APPEND RABBIT_PRIVATE_COMMON_LIBS log4cplus::log4cplus)
    # Install log4cplus configure file
    if(ANDROID)
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/log4cplus.conf
            DESTINATION assets/etc
                COMPONENT Runtime)
    else()
        INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/etc/log4cplus.conf
            DESTINATION etc
                COMPONENT Runtime)
    endif()
else()
    message("Download log4cplus from https://github.com/log4cplus/log4cplus")
endif()

list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES
    RabbitCommonTools.h
    RabbitCommonDir.h
    RabbitCommonStyle.h
    RabbitCommonLog.h
    RabbitRecentMenu.h
    )
list(APPEND RABBITCOMMON_HEAD_FILES
    ${RABBITCOMMON_INSTALL_HEAD_FILES}
    RabbitCommonRegister.h
    )
list(APPEND RABBITCOMMON_SOURCE_FILES
    RabbitCommonLog.cpp
    RabbitCommonDir.cpp
    RabbitCommonTools.cpp
    RabbitCommonRegister.cpp
    RabbitCommonStyle.cpp
    RabbitRecentMenu.cpp
    )
list(APPEND RABBITCOMMON_SOURCE_UI_FILES
    )

#翻译
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Qt5CorePatches.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Translations.cmake)

#设置资源文件
LIST(APPEND RCC_FILES
    Resource/ResourceRabbitCommon.qrc
    )

if(BUILD_VERSION)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS BUILD_VERSION="${BUILD_VERSION}")
else()
    message(WARNING "Please set BUILD_VERSION")
endif()

if(WIN32)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS WINDOWS UNICODE)
    list(APPEND RABBIT_PRIVATE_COMMON_LIBS Advapi32 Ole32 Shell32 netapi32 Ws2_32)
elseif(UNIX)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS UNIX)
endif()

list(APPEND RabbitCommon_PRIVATE_DEFINITIONS
    BUILD_ARCH="${BUILD_ARCH}"
    BUILD_PLATFORM="${BUILD_PLATFORM}")

if(MSVC)
    set(PRIVATE_OPTIONS "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
        "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
list(APPEND RABBITCOMMON_INSTALL_HEAD_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)

#需要的QT组件
LIST(APPEND _QT_COMPONENTS Gui Widgets)
if(ANDROID)
    LIST(APPEND _QT_COMPONENTS AndroidExtras)
endif()
if(NOT DEFINED QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${_QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${_QT_COMPONENTS})
        LIST(APPEND RABBIT_COMMON_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
get_filename_component(QT_INSTALL_DIR "${Qt${QT_VERSION_MAJOR}_DIR}/../../.." ABSOLUTE)
message("QT_INSTALL_DIR:${QT_INSTALL_DIR}")

foreach(l ${_QT_COMPONENTS})
    set(RabbitCommon_QT_COMPONENTS "${RabbitCommon_QT_COMPONENTS} ${l}")
endforeach()
message("RabbitCommon_QT_COMPONENTS:${RabbitCommon_QT_COMPONENTS}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommonUtils.cmake)
ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${RABBITCOMMON_SOURCE_FILES} ${RABBITCOMMON_SOURCE_UI_FILES} ${RCC_FILES} ${RABBITCOMMON_HEAD_FILES}
    INSTALL_HEADER_FILES ${RABBITCOMMON_INSTALL_HEAD_FILES}
    INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    DEFINITIONS ${RabbitCommon_DEFINITIONS}
    PRIVATE_DEFINITIONS ${RabbitCommon_PRIVATE_DEFINITIONS}
    LIBS ${RABBIT_COMMON_LIBS}
    PRIVATE_LIBS ${RABBIT_PRIVATE_COMMON_LIBS}
    PRIVATE_OPTIONS ${PRIVATE_OPTIONS}
    FEATURES cxx_std_11
    VERSION ${BUILD_VERSION}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_CMAKE_CONFIG_IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommonConfig.cmake.in
    )

if(UNIX OR MINGW)
    foreach(d ${RabbitCommon_DEFINITIONS})
        SET(RabbitCommon_DEFINITIONS_PC "${RabbitCommon_DEFINITIONS_PC} -D${d}")
    endforeach()

    # Install pc files
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommon.pc.in
        ${CMAKE_BINARY_DIR}/RabbitCommon.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/RabbitCommon.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# Install style files
if(ANDROID)
   INSTALL(DIRECTORY Resource/style/ DESTINATION "assets/data/style" COMPONENT Runtime)
   INSTALL(DIRECTORY QUIWidget/Resource/qss DESTINATION "assets/data/style" COMPONENT Runtime)
else()
   INSTALL(DIRECTORY Resource/style/ DESTINATION "data/style" COMPONENT Runtime)
   INSTALL(DIRECTORY QUIWidget/Resource/qss/ DESTINATION "data/style" COMPONENT Runtime)
endif()
