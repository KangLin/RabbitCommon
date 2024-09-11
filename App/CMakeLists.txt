# Author: Kang Lin(kl222@126.com)

project(RabbitCommonApp)

set(SOURCE_FILES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    )

set(SOURCE_UI_FILES MainWindow.ui)

#翻译
#include(${CMAKE_SOURCE_DIR}/cmake/Qt5CorePatches.cmake)
#include(${CMAKE_SOURCE_DIR}/cmake/Translations.cmake)
#include(${CMAKE_SOURCE_DIR}/cmake/RabbitCommonUtils.cmake)

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${RESOURCE_FILES} ${SOURCE_UI_FILES}
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
    )

if(NOT EXISTS ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}.conf)
    configure_file(${CMAKE_SOURCE_DIR}/Src/etc/log4qt.conf
        ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}.conf
        COPYONLY)

endif()
INSTALL(FILES ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}.conf
    DESTINATION etc
    COMPONENT Application)
if(NOT EXISTS ${CMAKE_BINARY_DIR}/etc/RabbitCommon_logqt.ini)
    configure_file(${CMAKE_SOURCE_DIR}/Src/etc/logqt.ini
        ${CMAKE_BINARY_DIR}/etc/RabbitCommon_logqt.ini
        COPYONLY)
endif()
INSTALL(FILES ${CMAKE_BINARY_DIR}/etc/RabbitCommon_logqt.ini
    DESTINATION etc
    COMPONENT Application)

if(NOT ANDROID AND UNIX)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/share/org.Rabbit.RabbitCommon.desktop
        DESTINATION "share/applications" COMPONENT Application)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/Src/Resource/image/App.png
        DESTINATION "share/pixmaps" COMPONENT Application)
    INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/share/RabbitCommonApp.sh
        DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Application)
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
