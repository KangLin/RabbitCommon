project(RabbitCommon)

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
    set(BUILD_ARCH ${ANDROID_ARCH})
endif()

#需要的QT组件
SET(QT_COMPONENTS Core Gui Widgets Network Xml)
if(ANDROID)
    SET(QT_COMPONENTS ${QT_COMPONENTS} AndroidExtras)
endif()
find_package(Qt5 COMPONENTS ${QT_COMPONENTS})
message("QT_VERSION:${Qt5_VERSION}")
if(Qt5_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
FOREACH(_COMPONENT ${QT_COMPONENTS})
    SET(QT_LIBRARIES ${QT_LIBRARIES} Qt5::${_COMPONENT})
ENDFOREACH()
get_filename_component(QT_INSTALL_DIR "${Qt5_DIR}/../../.." ABSOLUTE)
message("QT_INSTALL_DIR:${QT_INSTALL_DIR}")

#打开 qt 编译工具
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(INSTALL_HEAD_FILES "")
option(BUILD_ABOUT "Set to ON to build about function" ON)
if(BUILD_ABOUT)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_ABOUT)
    list(APPEND INSTALL_HEAD_FILES DlgAbout/DlgAbout.h)
    set(SOURCE_FILES ${SOURCE_FILES} DlgAbout/DlgAbout.cpp)
    set(SOURCE_UI_FILES ${SOURCE_UI_FILES} DlgAbout/DlgAbout.ui)
endif(BUILD_ABOUT)

option(BUILD_UPDATE "Set to ON to build update function" ON)
if(BUILD_UPDATE)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_UPDATE)
    list(APPEND INSTALL_HEAD_FILES FrmUpdater/FrmUpdater.h)
    set(SOURCE_FILES ${SOURCE_FILES} FrmUpdater/FrmUpdater.cpp)
    set(SOURCE_UI_FILES ${SOURCE_UI_FILES} FrmUpdater/FrmUpdater.ui)
endif(BUILD_UPDATE)
option(BUILD_ADMINAUTHORISER "Set to ON to build admin authoriser function()" ON)
if(BUILD_ADMINAUTHORISER)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_ADMINAUTHORISER)
    set(HEAD_FILES ${INSTALL_HEAD_FILES} AdminAuthoriser/adminauthoriser)
    set(SOURCE_FILES ${SOURCE_FILES} AdminAuthoriser/adminauthoriser.cpp)
    SET(HEAD_FILES ${HEAD_FILES} AdminAuthoriser/adminauthorization_p.h)
    if(WIN32)
        set(SOURCE_FILES ${SOURCE_FILES} AdminAuthoriser/adminauthorization_win.cpp)
    elseif(MAC)
        set(SOURCE_FILES ${SOURCE_FILES} AdminAuthoriser/adminauthorization_mac.cpp)
    elseif(NOT ANDROID AND UINX)
        set(SOURCE_FILES ${SOURCE_FILES} AdminAuthoriser/adminauthorization_x11.cpp)
    else()
        set(SOURCE_FILES ${SOURCE_FILES} AdminAuthoriser/adminauthorization_dummy.cpp)
    endif()
endif(BUILD_ADMINAUTHORISER)

set(INSTALL_HEAD_FILES
    ${INSTALL_HEAD_FILES}
    RabbitCommonTools.h
    RabbitCommonDir.h
    )
set(HEAD_FILES
    ${INSTALL_HEAD_FILES}
    ${HEAD_FILES}
    RabbitCommonRegister.h
    )
set(SOURCE_FILES
    ${SOURCE_FILES}
    RabbitCommonDir.cpp
    RabbitCommonTools.cpp
    RabbitCommonRegister.cpp
    )
set(SOURCE_UI_FILES ${SOURCE_UI_FILES})

#翻译
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Qt5CorePatches.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Translations.cmake)

GENERATED_QT_TRANSLATIONS(SOURCES ${SOURCE_FILES} ${SOURCE_UI_FILES}
    OUT_QRC TRANSLATIONS_QRC_FILES)
if("Debug" STREQUAL CMAKE_BUILD_TYPE)
    LIST(APPEND RCC_FILES 
        ${TRANSLATIONS_QRC_FILES}
        )
endif()

#设置资源文件
LIST(APPEND RCC_FILES
    Resource/ResourceRabbitCommon.qrc
    )

if(BUILD_VERSION)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS BUILD_VERSION="${BUILD_VERSION}")
else()
    message("Please set BUILD_VERSION")
endif()
if(WIN32)
    list(APPEND RabbitCommon_PRIVATE_DEFINITIONS WINDOWS UNICODE)
    set(LIBS Advapi32 Ole32 Shell32 netapi32)
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

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommonUtils.cmake)
ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${SOURCE_UI_FILES} ${RCC_FILES} ${HEAD_FILES}
    INSTALL_HEADER_FILES ${INSTALL_HEAD_FILES}
    DEFINITIONS ${RabbitCommon_DEFINITIONS}
    PRIVATE_DEFINITIONS ${RabbitCommon_PRIVATE_DEFINITIONS}
    LIBS ${LIBS} ${QT_LIBRARIES}
    PRIVATE_OPTIONS ${PRIVATE_OPTIONS}
    FEATURES cxx_std_11
    VERSION ${BUILD_VERSION})

# Generate export header files and export macro
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(${PROJECT_NAME})
string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
SET(INSTALL_HEAD_FILES ${INSTALL_HEAD_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h
    DESTINATION ${CMAKE_BINARY_DIR})

# See: http://www.it1352.com/478094.html
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

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

find_package(OpenSSL)
IF(OPENSSL_FOUND)
    IF(BUILD_SHARED_LIBS)
        INSTALL(FILES $<TARGET_FILE:OpenSSL::SSL>
                    $<TARGET_FILE:OpenSSL::Crypto>
                    DESTINATION "libs/${ANDROID_ABI}"
                        COMPONENT Runtime)
    endif()
ENDIF(OPENSSL_FOUND)
