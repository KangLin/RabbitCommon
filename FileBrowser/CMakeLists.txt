# Author: Kang Lin(kl222@126.com)

project(FileBrowser
    LANGUAGES CXX
    DESCRIPTION "File browser"
    )

include(GNUInstallDirs)

set(SOURCE_FILES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    Resource/FileBrowser.rc
)

set(SOURCE_UI_FILES MainWindow.ui)
set(APP_ID "io.github.KangLin.FileBrowser")
ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    IS_MACOSX_BUNDLE
    SOURCE_FILES ${SOURCE_FILES} ${RESOURCE_FILES} ${SOURCE_UI_FILES}
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
    VERSION ${RabbitCommon_VERSION}
    )

if(NOT EXISTS ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}_logqt.ini)
    configure_file(${CMAKE_SOURCE_DIR}/Src/etc/logqt.ini
        ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}_logqt.ini
        COPYONLY)
endif()
INSTALL_FILE(SOURCES ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}_logqt.ini
    DESTINATION etc
        COMPONENT Application)

if(NOT ANDROID AND UNIX)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/share/${APP_ID}.desktop
        DESTINATION "share/applications" COMPONENT Application)
    
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/Src/Resource/icons/rabbit-green/svg/browser.svg
        RENAME ${APP_ID}.svg
        DESTINATION "share/pixmaps" COMPONENT Application)
endif()

if(APPLE)
    # 设置 bundle 属性 (如果是 GUI 应用)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_ID}"
        MACOSX_BUNDLE_ICON_FILE "${APP_ID}.icns"
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        )
    file(COPY_FILE ${CMAKE_SOURCE_DIR}/Src/Resource/icons/rabbit-green/svg/browser.svg
        ${APP_ID}.svg)
    target_add_resources(${PROJECT_NAME} . ${APP_ID}.svg)
endif()

set(OTHER_FILES
    ${CMAKE_SOURCE_DIR}/License.md
    ${CMAKE_SOURCE_DIR}/Authors.md
    ${CMAKE_SOURCE_DIR}/Authors_zh_CN.md
    ${CMAKE_SOURCE_DIR}/ChangeLog.md
    ${CMAKE_SOURCE_DIR}/ChangeLog_zh_CN.md
    )
# Install other files
INSTALL_FILE(SOURCES ${OTHER_FILES}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT Application)
