CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
MESSAGE(STATUS "Found CMake ${CMAKE_VERSION}")

project(RabbitCommon)

if(POLICY CMP0083)
    cmake_policy(SET CMP0083 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

# 参见：
#   语义化版本: https://semver.org/lang/zh-CN/
SET(RabbitCommon_VERSION "2.2.6")
# Find Git Version Patch
IF(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    if(NOT GIT)
        SET(GIT $ENV{GIT})
    endif()
    if(NOT GIT)
        FIND_PROGRAM(GIT NAMES git git.exe git.cmd)
    endif()
    IF(GIT)
        EXECUTE_PROCESS(
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND ${GIT} describe --tags
            OUTPUT_VARIABLE GIT_VERSION  OUTPUT_STRIP_TRAILING_WHITESPACE
        )
	    if(NOT GIT_VERSION)
            EXECUTE_PROCESS(
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMAND ${GIT} rev-parse --short HEAD
                OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()
        SET(RabbitCommon_VERSION ${GIT_VERSION})
    ENDIF()
ENDIF()
message("RabbitCommon_VERSION ${PROJECT_NAME}:${RabbitCommon_VERSION}")
set(VERSION ${RabbitCommon_VERSION})

message("CMAKE_CXX_COMPILER_ARCHITECTURE_ID:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
message("CMAKE_SYSTEM_PROCESSOR:${CMAKE_SYSTEM_PROCESSOR}")

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT DEFINED CMAKE_BUILD_TYPE)
string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
if("debug" STREQUAL build_type)
    add_definitions(-D_DEBUG)
endif()

IF(MSVC)
    # This option is to enable the /MP switch for Visual Studio 2005 and above compilers
    OPTION(WIN32_USE_MP "Set to ON to build with the /MP option (Visual Studio 2005 and above)." ON)
    MARK_AS_ADVANCED(WIN32_USE_MP)
    IF(WIN32_USE_MP)
        #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        add_compile_options(/MP)
    ENDIF(WIN32_USE_MP)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
ENDIF(MSVC)

IF(MINGW)
    add_compile_options("-std=c++0x")
ENDIF()

SET(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libs")
if (BUILD_SHARED_LIBS)
	add_definitions(-DBUILD_SHARED_LIBS)
    if (CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
       # Just setting CMAKE_POSITION_INDEPENDENT_CODE should be enough to set
       # -fPIC for GCC but sometimes it still doesn't get set, so make sure it
       # does.
       add_definitions("-fPIC")
    endif()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

include(CMakePackageConfigHelpers)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckCCompilerFlag)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckCSourceCompiles)
include(CMakeDependentOption)

#CHECK_INCLUDE_FILE_CXX("string" HAVE_STRING_H)
#check_include_file("math.h" HAVE_MATH_H)

#check_function_exists("fabs" HAVE_FABS)

set(BUILD_PLATFORM "${CMAKE_SYSTEM_NAME}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
message("QT_VERSION_MAJOR:${QT_VERSION_MAJOR}")
#需要的QT组件
SET(QT_COMPONENTS Core Gui Widgets Network Xml)
if(QT_VERSION_MAJOR LESS 6 AND ANDROID)
    message(FATAL_ERROR "Qt must great 6.0.0 for android")
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
        SET(QT_LIBRARIES ${QT_LIBRARIES} Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Don't find Qt${QT_VERSION_MAJOR}")
endif()
get_filename_component(QT_INSTALL_DIR "${Qt${QT_VERSION_MAJOR}_DIR}/../../.." ABSOLUTE)
message("QT_INSTALL_DIR:${QT_INSTALL_DIR}")

#打开 qt 编译工具
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)

option(BUILD_TEST "Set to ON to build tests" OFF)
add_subdirectory(Src)
add_subdirectory(FileBrowser)
option(BUILD_APP "Set to ON to build application" ON)
if(BUILD_APP)
    add_subdirectory(App)
endif()
if(BUILD_TEST)
    enable_testing() # Enable testing only works in root scope
    add_subdirectory(Test)
endif(BUILD_TEST)

message(STATUS "Build tests: ${BUILD_TEST}")
message(STATUS "Build app: ${BUILD_APP}")
if(BUILD_APP)
    message(STATUS "Build about function: ${BUILD_ABOUT}")
    message(STATUS "BUild update function: ${BUILD_UPDATE}")
endif()
message(STATUS "Build shared library: ${BUILD_SHARED_LIBS}")

option(BUILD_DOCS "Generating API documentation" OFF)
message(STATUS "Build documents: ${BUILD_DOCS}")
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Doxygen)
        file(COPY ${CMAKE_SOURCE_DIR}/documents/doxygen/index.html
            DESTINATION ${CMAKE_BINARY_DIR}/Doxygen)
        foreach(OUTPUT_LANGUAGE English Chinese)
            set(MAIN_README "README.md")
            if(OUTPUT_LANGUAGE STREQUAL "Chinese")
                set(MAIN_README "README_zh_CN.md")
            endif()            
            configure_file(${CMAKE_SOURCE_DIR}/documents/doxygen/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile_${OUTPUT_LANGUAGE} @ONLY)
            add_custom_target(doc_${OUTPUT_LANGUAGE} ALL
                ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile_${OUTPUT_LANGUAGE} # &> doxygen.log
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen" VERBATIM
                )
        endforeach()
        install(DIRECTORY ${CMAKE_BINARY_DIR}/Doxygen/
            DESTINATION ${CMAKE_INSTALL_DOCDIR}/${OUTPUT_LANGUAGE}
                COMPONENT Runtime)
    endif(DOXYGEN_FOUND)
endif(BUILD_DOCS)

include(cmake/RabbitCommonCPack.cmake)
