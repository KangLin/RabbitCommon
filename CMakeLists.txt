if(ANDROID)
    cmake_minimum_required(VERSION 3.19)
else()
    cmake_minimum_required(VERSION 2.8)
endif()

project(RabbitCommon)

if(POLICY CMP0083)
    cmake_policy(SET CMP0083 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

SET(RabbitCommon_VERSION "v1.0.09")
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
set(BUILD_PLATFORM "${CMAKE_SYSTEM_NAME}")

# ----------------------------------------------------------------------------
# Detect compiler and target platform architecture
# ----------------------------------------------------------------------------
if(NOT ANDROID)
    if(X86_64 OR CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(HOST_ARCH x86_64)
    elseif(X86 OR CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(HOST_ARCH x86)
    endif()
else()
    set(HOST_ARCH ${ANDROID_ARCH})
endif()
message("Host architecture:${HOST_ARCH}")
message("CMAKE_CXX_COMPILER_ARCHITECTURE_ID:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
set(BUILD_ARCH ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID})

add_definitions(-DRabbitCommon_VERSION="${RabbitCommon_VERSION}")
add_definitions(-DBUILD_ARCH="${BUILD_ARCH}")
add_definitions(-DBUILD_PLATFORM="${BUILD_PLATFORM}")

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
include(GNUInstallDirs)
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
    set(BUILD_ARCH ${CMAKE_ANDROID_ARCH})
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
message("QT_VERSION_MAJOR:${QT_VERSION_MAJOR}")
#需要的QT组件
SET(QT_COMPONENTS Core Gui Widgets Network Xml)
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 6.0.0 AND ANDROID)
    SET(QT_COMPONENTS ${QT_COMPONENTS} AndroidExtras)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
        SET(QT_LIBRARIES ${QT_LIBRARIES} Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
get_filename_component(QT_INSTALL_DIR "${Qt${QT_VERSION_MAJOR}_DIR}/../../.." ABSOLUTE)
message("QT_INSTALL_DIR:${QT_INSTALL_DIR}")

#打开 qt 编译工具
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(Src)
option(BUILD_APP "Set to ON to build application" ON)
if(BUILD_APP)
    add_subdirectory(App)
endif()
option(BUILD_TESTS "Set to ON to build tests" ON)
if(BUILD_TESTS)    
    add_subdirectory(Tests)
endif(BUILD_TESTS)

set(OTHER_FILES
    License.md
    Authors.md
    Authors_zh_CN.md
    ChangeLog.md
    )

# Install other files
IF(ANDROID)
    INSTALL(FILES ${OTHER_FILES} DESTINATION "assets" COMPONENT Runtime)
else()
    INSTALL(FILES ${OTHER_FILES} DESTINATION "." COMPONENT Runtime)
endif()

## Create will be delete files
#CONFIGURE_FILE(
#    "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
#    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
#    IMMEDIATE @ONLY)
## Create unistall target
#ADD_CUSTOM_TARGET(uninstall
#    "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
#    DEPENDS uninstall-runtime)

message("Build app:${BUILD_APP}")
if(BUILD_APP)
    message("Build about function:${BUILD_ABOUT}")
    message("BUild update function:${BUILD_UPDATE}")
endif()
message("Build shared library:${BUILD_SHARED_LIBS}")

option(BUILD_DOCS "Generating API documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Doxygen)
        foreach(OUTPUT_LANGUAGE English Chinese)
            set(MAIN_README "README.md")
            if(OUTPUT_LANGUAGE STREQUAL "Chinese")
                set(MAIN_README "README_zh_CN.md")
            endif()            
            configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile_${OUTPUT_LANGUAGE} @ONLY)
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
