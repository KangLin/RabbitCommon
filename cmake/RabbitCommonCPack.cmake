# Author: Kang Lin <kl222@126.com>

# 参考： https://zhuanlan.zhihu.com/p/377131996
#       https://github.com/medInria/medinria-superproject/blob/master/packaging/Packaging.cmake

# Generate .txt license file for CPack (PackageMaker requires a file extension)
configure_file(${CMAKE_SOURCE_DIR}/License.md ${CMAKE_BINARY_DIR}/LICENSE.txt)

set(CPACK_SOURCE_IGNORE_FILES
    ${CMAKE_SOURCE_DIR}/build
    ${CMAKE_SOURCE_DIR}/.cache
    ${CMAKE_SOURCE_DIR}/.git
    ${CMAKE_SOURCE_DIR}/.github
    ${CMAKE_SOURCE_DIR}/.gitignore
    ${CMAKE_SOURCE_DIR}/.dockerignore
    ${CMAKE_SOURCE_DIR}/CMakeCache.txt)

set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
if(NOT DEFINED CPACK_PACKAGE_NAME)
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
endif()

if(NOT DEFINED CPACK_SYSTEM_NAME)
    if(MSVC)
        set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
    else()
        set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()

if(NOT DEFINED CPACK_TOPLEVEL_TAG)
    if(MSVC)
        set(CPACK_TOPLEVEL_TAG "${CMAKE_SYSTEM_NAME}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
    else()
        set(CPACK_TOPLEVEL_TAG "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()

# 设置安装包的文件名
string(TOLOWER ${CPACK_PACKAGE_NAME} CPACK_PACKAGE_NAME_lower)
# 设置二进制安装包的文件名
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_lower}_${CPACK_PACKAGE_VERSION}_${CPACK_SYSTEM_NAME}_setup")
# 设置源码安装包的文件名
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_lower}_${CPACK_PACKAGE_VERSION}_source")

#set(CPACK_PACKAGE_CHECKSUM "MD5")

############### NSIS ###################
if(WIN32)
    
    #NSIS 安装程序提供给最终用户的默认安装目录位于此根目录下。提供给最终用户的完整目录是：${CPACK_NSIS_INSTALL_ROOT}/${CPACK_PACKAGE_INSTALL_DIRECTORY}
    #set(CPACK_NSIS_INSTALL_ROOT "$LOCALAPPDATA")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    #设置安装包的编码。默认为：lzma
    #set(CPACK_NSIS_COMPRESSOR lzma)
    
    #如果设置为 ON，则安装程序中将出现一个额外的页面，允许用户选择是否应将程序目录添加到系统 PATH 变量中。
    set(CPACK_NSIS_MODIFY_PATH OFF)

endif()
############### NSIS ###################


############### Debian ###################
if(UNIX AND (NOT (MACOS AND APPLE)) AND (NOT ANDROID))

    set(CPACK_DEBIAN_PACKAGE_DEBUG ON)

    ##### 产生 control 文件 #####

    # 设置 Package 字段（自动转换成小写）。 默认使用 CPACK_PACKAGE_NAME
    #set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})

    # 更改 CPACK 包的默认安装路径前缀。
    # 或者在 cpack 时传递参数 -DCPACK_PACKAGING_INSTALL_PREFIX=/opt
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${CPACK_PACKAGE_NAME}")

    # 默认值： CPACK_PACKAGE_VERSION
    string(REPLACE "v" "" _DEBIAN_VERSION ${CPACK_PACKAGE_VERSION})
    set(CPACK_DEBIAN_PACKAGE_VERSION ${_DEBIAN_VERSION})

    # 设置包的依赖
    # set(CPACK_DEBIAN_PACKAGE_DEPENDS)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    # May be set to a list of directories that will be given to dpkg-shlibdeps via its -l option. These will be searched by dpkg-shlibdeps in order to find private shared library dependencies.
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS ${QT_INSTALL_DIR}/lib;${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_SYSTEM_NAME}/DEB/${CPACK_PACKAGE_FILE_NAME}${CPACK_PACKAGING_INSTALL_PREFIX}/lib)
    message("CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS:${CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS}")
    set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
    #set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY ">=")

    set(CPACK_DEBIAN_PACKAGE_PREDEPENDS "debhelper (>= 13), qt6-tools-dev, qt6-tools-dev-tools,
        qt6-base-dev, qt6-base-dev-tools, qt6-qpa-plugins, qt6-l10n-tools,
        qt6-translations-l10n, qt6-scxml-dev, qt6-webengine-dev,
        qt6-webengine-dev-tools, qt6-multimedia-dev, libsqlite3-dev")

endif()
############### Debian ###################

include(CPackComponent)
include(CPack)

SET(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT Runtime)
if(CMAKE_MFC_FLAG)
    set(CMAKE_INSTALL_MFC_LIBRARIES TRUE)
endif()
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE)
endif()
if(LOWER_BUILD_TYPE STREQUAL "debug")
    set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
endif()
set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT DependLibraries)
include(InstallRequiredSystemLibraries)

cpack_add_component(DependLibraries
    DISPLAY_NAME  "DependLibraries"
    DESCRIPTION   "Depend Libraries"
    )

cpack_add_component(Runtime
    DISPLAY_NAME  "Runtime"
    DESCRIPTION   "Runtime"
    DEPENDS DependLibraries
    )

cpack_add_component(Development
    DISPLAY_NAME  "Development"
    DESCRIPTION   "Development"
	DEPENDS Runtime
    )

cpack_add_component(Application
    DISPLAY_NAME  "Application"
    DESCRIPTION   "Application"
	DEPENDS Runtime
    )

cpack_add_component(Plugin
    DISPLAY_NAME  "Plugin"
    DESCRIPTION   "Plugin"
	DEPENDS Runtime
    )

