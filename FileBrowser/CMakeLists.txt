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

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${RESOURCE_FILES} ${SOURCE_UI_FILES}
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
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
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/share/org.Rabbit.FileBrower.desktop
        DESTINATION "share/applications" COMPONENT Application)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/Src/Resource/icons/rabbit-green/svg/browser.svg
        RENAME org.Rabbit.FileBrower.svg
        DESTINATION "share/pixmaps" COMPONENT Application)
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
