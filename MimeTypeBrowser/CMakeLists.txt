# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(MimeTypeBrowser
    LANGUAGES CXX
    DESCRIPTION "Mime type browser"
    )

include(GNUInstallDirs)

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp mainwindow.h
    mimetypemodel.cpp mimetypemodel.h
)

set(APP_ID "io.github.KangLin.MimeTypeBrowser")

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${RESOURCE_FILES} ${SOURCE_UI_FILES}
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    VERSION ${RabbitCommon_VERSION}
    )

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
endif()