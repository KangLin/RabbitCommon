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
    BUNDLE_GUI_IDENTIFIER ${APP_ID}
    BUNDLE_ICON_FILE ${APP_ID}
    )
