# Author: Kang Lin(kl222@126.com)

project(GenerateUpdateFile)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
message("QT_VERSION_MAJOR:${QT_VERSION_MAJOR}")
#需要的QT组件
SET(QT_COMPONENTS Core)
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

message("RabbitCommon_VERSION ${PROJECT_NAME}:${RabbitCommon_VERSION}")

set(SOURCE_FILES
    main.cpp
    GenerateUpdateFile.cpp
    )
set(HEADER_FILES
    GenerateUpdateFile.h
    )

#翻译
include(${CMAKE_SOURCE_DIR}/cmake/Qt5CorePatches.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Translations.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/RabbitCommonUtils.cmake)

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES} ${SOURCE_UI_FILES}
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
    )
