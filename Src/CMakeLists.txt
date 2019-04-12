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
    set(BUILD_ARCH ${CMAKE_ANDROID_ARCH})
endif()

#需要的QT组件
SET(QT_COMPONENTS Core Gui Widgets Xml Network Multimedia)
find_package(Qt5 COMPONENTS ${QT_COMPONENTS})

#打开 qt 编译工具
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)

FOREACH(_COMPONENT ${QT_COMPONENTS})
    SET(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5${_COMPONENT}_LIBRARIES})
ENDFOREACH()

SET(QT_INSTALL_DIR "${Qt5_DIR}/../../..")

set(INSTALL_HEAD_FILES
    FrmUpdater/FrmUpdater.h 
    DlgAbout/DlgAbout.h
    Tools.h
    rabbitcommon_export.h
    rabbitcommon_export_windows.h
    rabbitcommon_export_linux.h
    )
set(HEAD_FILES
    ${INSTALL_HEAD_FILES}
    GlobalDir.h 
    )
set(SOURCE_FILES
    DlgAbout/DlgAbout.cpp
    FrmUpdater/FrmUpdater.cpp
    GlobalDir.cpp
    Tools.cpp
    )
set(SOURCE_UI_FILES
    FrmUpdater/FrmUpdater.ui
    DlgAbout/DlgAbout.ui
    )

#翻译
include(${CMAKE_SOURCE_DIR}/cmake/Qt5CorePatches.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Translations.cmake)

#设置资源文件
SET(RCC_FILES
    Resource/Resource.qrc
    ${TRANSLATIONS_RESOURCE_FILES}
    )

if(WIN32)
    #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif()
IF(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
ENDIF()
add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEAD_FILES}
    ${SOURCE_UI_FILES} ${RCC_FILES})
#add_dependencies(${PROJECT_NAME} translations_${TRANSLATIONS_NAME})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

if(BUILD_VERSION)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DBUILD_VERSION="${BUILD_VERSION}")
else()
    message("Please set BUILD_VERSION")
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DBUILD_ARCH="${BUILD_ARCH}"
    -DBUILD_PLATFORM="${BUILD_PLATFORM}")
set_target_properties(${PROJECT_NAME} PROPERTIES 
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}")
# See: http://www.it1352.com/478094.html
#target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(${PROJECT_NAME})
string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
SET(INSTALL_HEAD_FILES ${INSTALL_HEAD_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)

export(
    TARGETS
        ${PROJECT_NAME}
    FILE
        ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}.cmake  # not going to be installed
)

# Install head files
#INSTALL(FILES ${INSTALL_HEAD_FILES} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
set_property(
    TARGET
        ${PROJECT_NAME}
    PROPERTY
        PUBLIC_HEADER "${INSTALL_HEAD_FILES}"
)

# Install target
INSTALL(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
INSTALL(
        EXPORT ${PROJECT_NAME}
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/
        NAMESPACE ${PROJECT_NAME}::
    )
# Install cmake files
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/LunarCalendarConfig.cmake.in
#    ${CMAKE_BINARY_DIR}/LunarCalendarConfig.cmake @ONLY)
#install(FILES ${CMAKE_BINARY_DIR}/LunarCalendarConfig.cmake DESTINATION .)
#write_basic_package_version_file(
#    "${CMAKE_BINARY_DIR}/LunarCalendarConfigVersion.cmake"
#    VERSION ${BUILD_VERSION}
#    COMPATIBILITY AnyNewerVersion)
#install(FILES ${CMAKE_BINARY_DIR}/LunarCalendarConfigVersion.cmake
#    DESTINATION .)
#if(NOT MSVC)
#    # Install pc files
#    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/LunarCalendar.pc.in
#        ${CMAKE_BINARY_DIR}/LunarCalendar.pc @ONLY)
#    install(FILES ${CMAKE_BINARY_DIR}/LunarCalendar.pc
#        DESTINATION lib/pkgconfig)
#endif()
IF(WIN32 AND BUILD_SHARED_LIBS)
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
