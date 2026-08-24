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

if(ANDROID)
    if(NOT RabbitCommon_VERSION)
        message(WARNING "RabbitCommon_VERSION is empty, using default version")
        set(RabbitCommon_VERSION "1.0.0")
    endif()

    # 确保版本号格式正确
    if(NOT RabbitCommon_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+")
        message(WARNING "Version format is not 'x.y.z', using fallback parsing")
    endif()

    # 使用方案一的解析逻辑
    string(REGEX MATCHALL "[0-9]+" VERSION_PARTS "${RabbitCommon_VERSION}")
    list(LENGTH VERSION_PARTS PARTS_COUNT)

    if(PARTS_COUNT GREATER_EQUAL 3)
        list(GET VERSION_PARTS 0 VERSION_MAJOR)
        list(GET VERSION_PARTS 1 VERSION_MINOR)
        list(GET VERSION_PARTS 2 VERSION_PATCH)
    else()
        # 不足3段时补齐
        set(VERSION_MAJOR 1)
        set(VERSION_MINOR 0)
        set(VERSION_PATCH 0)
    endif()

    math(EXPR QT_ANDROID_VERSION_CODE "${VERSION_MAJOR}*10000 + ${VERSION_MINOR}*100 + ${VERSION_PATCH}")
    set(QT_ANDROID_VERSION_NAME "${RabbitCommon_VERSION}")

    message(STATUS "${PROJECT_NAME} Version Code: ${QT_ANDROID_VERSION_CODE}, Version Name: ${QT_ANDROID_VERSION_NAME}")

    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_ANDROID_PACKAGE_NAME "${APP_ID}"
        QT_ANDROID_APP_NAME "@string/app_name"
        QT_ANDROID_APP_ICON "@drawable/icon"
        QT_ANDROID_VERSION_CODE ${QT_ANDROID_VERSION_CODE}
        QT_ANDROID_VERSION_NAME ${QT_ANDROID_VERSION_NAME}
        #QT_ANDROID_MIN_SDK_VERSION 28
        QT_ANDROID_TARGET_SDK_VERSION 34
        )
    qt_add_android_permission(${PROJECT_NAME}
        NAME android.permission.INTERNET
    )
    qt_add_android_permission(${PROJECT_NAME}
        NAME android.permission.WRITE_EXTERNAL_STORAGE
    )
    qt_add_android_permission(${PROJECT_NAME}
        NAME android.permission.READ_EXTERNAL_STORAGE
    )
    qt_add_android_permission(${PROJECT_NAME}
        NAME android.permission.REQUEST_INSTALL_PACKAGES
    )
elseif(UNIX)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/share/${APP_ID}.desktop
        DESTINATION "share/applications" COMPONENT Application)
    
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/Src/Resource/icons/rabbit-green/scalable/apps/browser.svg
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
    file(COPY_FILE ${CMAKE_SOURCE_DIR}/Src/Resource/icons/rabbit-green/scalable/apps/browser.svg
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
