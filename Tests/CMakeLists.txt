# Author: Kang Lin(kl222@126.com)

project(RabbitCommonTests)

set(SOURCE_FILES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    )

set(SOURCE_UI_FILES MainWindow.ui)

#翻译
include(${CMAKE_SOURCE_DIR}/cmake/Qt5CorePatches.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Translations.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/RabbitCommonUtils.cmake)

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
if(NOT EXISTS ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}_logqt.ini)
    configure_file(${CMAKE_SOURCE_DIR}/Src/etc/logqt.ini
        ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}_logqt.ini
        COPYONLY)
endif()

if(NOT ANDROID AND UNIX)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/share/org.Rabbit.RabbitCommon.desktop
        DESTINATION "share/applications" COMPONENT Runtime)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/Src/Resource/image/App.png
        DESTINATION "share/pixmaps" COMPONENT Runtime)
    INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/share/RabbitCommonTests.sh
        DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endif()
