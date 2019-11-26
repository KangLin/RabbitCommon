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

option(BUILD_ABOUT "Set to ON to build about function" ON)
if(BUILD_ABOUT)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_ABOUT)
    set(INSTALL_HEAD_FILES ${INSTALL_HEAD_FILES} DlgAbout/DlgAbout.h)
    set(SOURCE_FILES ${SOURCE_FILES} DlgAbout/DlgAbout.cpp)
    set(SOURCE_UI_FILES ${SOURCE_UI_FILES} DlgAbout/DlgAbout.ui)
endif(BUILD_ABOUT)

option(BUILD_UPDATE "Set to ON to build update function" ON)
if(BUILD_UPDATE)
    list(APPEND RabbitCommon_DEFINITIONS HAVE_UPDATE)
    set(INSTALL_HEAD_FILES ${INSTALL_HEAD_FILES} FrmUpdater/FrmUpdater.h)
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

#设置资源文件
SET(RCC_FILES
    Resource/ResourceRabbitCommon.qrc
    ${TRANSLATIONS_RESOURCE_FILES}
    )

add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEAD_FILES}
    ${SOURCE_UI_FILES} ${RCC_FILES})
# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    PUBLIC_HEADER "${INSTALL_HEAD_FILES}" # Install head files
    )
if(BUILD_VERSION)
    target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_VERSION="${BUILD_VERSION}")
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${BUILD_VERSION})
else()
    message("Please set BUILD_VERSION")
endif()
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WINDOWS UNICODE)
    set(LIBS Advapi32 Ole32 Shell32 netapi32)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNIX)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ${QT_LIBRARIES} ${LIBS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${RabbitCommon_DEFINITIONS})
foreach(d ${RabbitCommon_DEFINITIONS})
    SET(RabbitCommon_DEFINITIONS_PC "${RabbitCommon_DEFINITIONS_PC} -D${d}")
endforeach()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    BUILD_ARCH="${BUILD_ARCH}"
    BUILD_PLATFORM="${BUILD_PLATFORM}")
IF(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "$<$<C_COMPILER_ID:MSVC>:/utf-8>" 
        "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
ENDIF()

# See: http://www.it1352.com/478094.html
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

# Generate export header files and export macro
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(${PROJECT_NAME})
string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
SET(INSTALL_HEAD_FILES ${INSTALL_HEAD_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h
    DESTINATION ${CMAKE_BINARY_DIR})

# Install target
if(ANDROID)
    INSTALL(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config
            RUNTIME DESTINATION "libs/${ANDROID_ABI}"
                COMPONENT Runtime
            LIBRARY DESTINATION "libs/${ANDROID_ABI}"
                COMPONENT Runtime
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RabbitCommon
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RabbitCommon
            )
elseif(WIN32)
    INSTALL(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT Runtime
            LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT Runtime
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RabbitCommon
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RabbitCommon
            )
else()
    INSTALL(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT Runtime
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
                COMPONENT Runtime
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RabbitCommon
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RabbitCommon
            )
endif()
# export build tree configure
export(TARGETS ${PROJECT_NAME}
       APPEND FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
)
# Install cmake configure files
install(
        EXPORT ${PROJECT_NAME}Config
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${BUILD_VERSION}
    COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")

if(UNIX OR MINGW)
    # Install pc files
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RabbitCommon.pc.in
        ${CMAKE_BINARY_DIR}/RabbitCommon.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/RabbitCommon.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

IF("Release" STREQUAL CMAKE_BUILD_TYPE AND WIN32 AND BUILD_SHARED_LIBS)
    IF(CMAKE_BUILD_TYPE MATCHES Release AND MINGW)
        #windeployqt 分发时，是根据是否 strip 来判断是否是 DEBUG 版本,而用mingw编译时,qt没有自动 strip
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND strip "$<TARGET_FILE:${PROJECT_NAME}>"
            )
    ENDIF()

    #注意 需要把 ${QT_INSTALL_DIR}/bin 加到环境变量PATH中  
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${QT_INSTALL_DIR}/bin/windeployqt"
        --compiler-runtime
        --verbose 7
        "$<TARGET_FILE:${PROJECT_NAME}>"
        )
    #INSTALL(DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>/" DESTINATION "${CMAKE_INSTALL_BINDIR}")
ENDIF()
