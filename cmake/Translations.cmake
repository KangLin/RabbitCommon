# Author: Kang Lin <kl222@126.com>

#GENERATED_QT_TRANSLATIONS 函数：生成 qt 翻译
#+ 功能：
#  - 生成或更新翻译源文件(.ts)，需要手动执行目标 translations_update_${PARA_TARGET} 
#  - 生成翻译文件(.qm)
#  - 生成翻译资源文件(.qrc)，放到参数 OUT_QRC 指定的变量中
#  - 安装翻译文件(.qm)到安装目录。目录结构详见后面说明
#+ 参数：
#  - [必须] SOURCES: 要理新的源文件。默认使用变量 SOURCE_FILES 和 SOURCE_UI_FILES 之中的源文件。
#  - [可选] ALL: 自动做为 ALL 的依赖
#  - [可选] NAME: 生成的翻译源文件(.ts)文件名前缀，默认值 ${PROJECT_NAME}。
#    **注意**：翻译资源名为此名字加上前缀 translations_ ,它也可以由 OUT_QRC_NAME 参数指定的变量得到
#  - [可选] TARGET: 生成的翻译对象，默认值 ${PROJECT_NAME}。
#    **注意**：翻译对象为此名字加上前缀 translations_ 
#  - [可选] TSDIR: 翻译源文件(.ts)存放的目录，默认值：${CMAKE_CURRENT_SOURCE_DIR}/Resource/Translations
#  - [可选] QM_INSTALL_DIR: 指定翻译文件（.qm）的安装目录。默认值：${CMAKE_INSTAL_PREFIX}/translations
#+ 输出值参数：
#  - [可选] OUT_QRC: 生成的翻译资源文件(.qrc) 变量。默认为：TRANSLATIONS_RESOURCE_FILES
#    如果需要使用翻译资源文件，则把它加入到add_executable 或 add_library 中。
#  - [可选] OUT_QRC_NAME: 翻译资源文件(.qrc)名变量，它用于代码使用库中的资源时，
#    调用 Q_INIT_RESOURCE 初始化此翻译资源
#+ 使用：
#  - 在 CMakeLists.txt加入包含此文件

#        include(Translations.cmake)

#  - 调用 GENERATED_QT_TRANSLATIONS 函数
#    + [必选] 设置 SOURCES 参数为要更新的源文件
#    + [可选] 设置 OUT_QRC 参数为接收资源文件名变量。默认为：TRANSLATIONS_RESOURCE_FILES
#    + [可选] 设置 NAME 参数为翻译源文件(.ts)文件名的前缀，默认值是目标名 ${PROJECT_NAME}。
#            **注意**：翻译资源名为此名字加上前缀 translations_ 。这个也可以由 OUT_QRC_NAME 参数指定的变量得到
#    + [可选] 设置 TSDIR 参数为翻译源文件(.ts)生成的目录。默认值是 ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Translations
#    + [可选] 设置 QM_INSTALL_DIR 参数为翻译文件（.qm）的安装目录。默认值：${CMAKE_INSTAL_PREFIX}/translations
#  - 如果要使用翻译资源文件，
#    则把输出参数 OUT_QRC 后的变量值加入到 add_executable 或 add_library 中。

#        GENERATED_QT_TRANSLATIONS(SOURCES ${SOURCE_FILES} ${SOURCE_UI_FILES}
#            OUT_QRC TRANSLATIONS_RESOURCE_FILES)
#        add_executable(${PROJECT_NAME} ${TRANSLATIONS_RESOURCE_FILES})

#    在C++代码 main 中加入下列代码初始化翻译资源：

#        Q_INIT_RESOURCE(translations_${PROJECT_NAME}); 

#  - 如果不需要把翻译放入到资源文件中。在代码中从文件系统加载翻译。详见后面的程序的安装目录。
        
#        QTranslator translator;
#        translator.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
#                   + "/" + QCoreApplication::applicationName() + "_" + QLocale::system().name() + ".qm");
#        QCoreApplication::installTranslator(&translator);

#  - 增加目标依赖（可选，如果设置了 ALL 参数，则可以不需要）： 

#        add_dependencies(${TRANSLATIONS_NAME} translations_${TRANSLATIONS_NAME})

#  - 在源码 main 函数中加入下列代码。
#    + 初始化翻译资源。如果是 DEBUG，需要加入宏定义 _DEBUG . 必须使用 -DCMAKE_BUILD_TYPE=Debug

#        // 如果使用了翻译资源文件，则必须加上此步，初始化翻译资源
#        // 资源文件名为 translations_ 加上 “设置的 NAME”
#        #if defined (_DEBUG)
#           Q_INIT_RESOURCE(translations_${PROJECT_NAME});
#        #endif

#        // 如果不再使用翻译资源文件，则需要释放翻译资源
#        #if defined (_DEBUG)
#           Q_CLEANUP_RESOURCE(translations_${PROJECT_NAME});
#        #endif

#    + 安装翻译

#        // 安装翻译
#        QTranslator translator;
#        translator.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
#                   + "/" + QCoreApplication::applicationName() + "_" + QLocale::system().name() + ".qm");
#        QCoreApplication::installTranslator(&translator);

#        //结束时释放翻译资源
#        QCoreApplication::removeTranslator(&translator)

#  - 完整的例子：
#    + CMakeLists.txt
  
#        #翻译
#        include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Translations.cmake)
        
#        GENERATED_QT_TRANSLATIONS(SOURCES ${SOURCE_FILES} ${SOURCE_UI_FILES}
#            OUT_QRC TRANSLATIONS_RESOURCE_FILES)
#        if("Debug" STREQUAL CMAKE_BUILD_TYPE)
#            LIST(APPEND QRC_FILE 
#                ${TRANSLATIONS_RESOURCE_FILES}
#                )
#        endif()
#        add_executable(${PROJECT_NAME} ${QRC_FILE})
#        # 增加依赖（可选）
#        add_dependencies(${PROJECT_NAME} translations_${PROJECT_NAME})

#    + 源码文件(main.c)

#        // 如果使用了翻译资源文件，则必须加上此步，初始化翻译资源
#        // 资源文件名为 translations_ 加上 “设置的 NAME”
#        #ifdef _DEBUG
#            Q_INIT_RESOURCE(translations_${PROJECT_NAME});
#        #endif 
#        // 安装翻译对象
#        QTranslator translator;
#        translator.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
#                   + "/" + QCoreApplication::applicationName() + "_" + QLocale::system().name() + ".qm");
#        QCoreApplication::installTranslator(&translator);

#        ......

#        //结束时释放翻译资源
#        QCoreApplication::removeTranslator(&translator)
#        #if defined (_DEBUG) || defined (DEBUG)
#           Q_CLEANUP_RESOURCE(translations_${PROJECT_NAME});
#        #endif

# Debug 翻译资源做为资源文件嵌入程序

# Android 翻译 qm 文件放在 assets 中
#
# Android:
#     assets                                       GetDirApplicationInstallRoot()  (Only read)
#        |- translations                           GetDirTranslations()
#        |   |- ${TRANSLATIONS_NAME}_zh_CN.qm
#        |   |- ${TRANSLATIONS_NAME}_zh_TW.qm
#        |- plugins
#             |- translations                      GetDirPluginsTranslation()
#             |   |- ${TRANSLATIONS_NAME}_zh_CN.qm
#             |   |- ${TRANSLATIONS_NAME}_zh_TW.qm
#             |- type
#                 |- plugin1
#                     |- translation               GetDirPluginsTranslation("plugins/type/plugin1")
#                     |   |- ${TRANSLATIONS_NAME}_zh_CN.qm
#                     |   |- ${TRANSLATIONS_NAME}_zh_TW.qm
#
# 其它系统发行模式下，做为文件放在程序的安装目录 translations 目录下
# 程序的安装目录：
#   AppRoot |                                        GetDirApplicationInstallRoot()  (Only read)
#           |- bin
#           |   |- App.exe
#           |- lib
#           |
#           |- translations                          GetDirTranslations()
#           |   |- ${TRANSLATIONS_NAME}_zh_CN.qm
#           |   |- ${TRANSLATIONS_NAME}_zh_TW.qm
#           |- plugins
#               |- translations                      GetDirPluginsTranslation()
#               |   |- ${TRANSLATIONS_NAME}_zh_CN.qm
#               |   |- ${TRANSLATIONS_NAME}_zh_TW.qm
#               |- type
#                   |- plugin1
#                       |- translation               GetDirPluginsTranslation("plugins/type/plugin1")
#                       |   |- ${TRANSLATIONS_NAME}_zh_CN.qm
#                       |   |- ${TRANSLATIONS_NAME}_zh_TW.qm
#
# 源码目录：
#   SourceRoot |
#              |- App
#              |   |- Resource
#              |        |-Translations
#              |             |- ${TRANSLATIONS_NAME}_zh_CN.ts
#              |             |- ${TRANSLATIONS_NAME}_zh_TW.ts
#              |- cmake
#              |   |- Translations.cmake
#              |- Src
#              |   |- Resource
#              |         |-Translations
#              |              |- ${TRANSLATIONS_NAME}_zh_CN.ts
#              |              |- ${TRANSLATIONS_NAME}_zh_TW.ts
#              |- plugins
#                   |- translations
#                   |   |- ${TRANSLATIONS_NAME}_zh_CN.ts
#                   |   |- ${TRANSLATIONS_NAME}_zh_TW.ts
#                   |- type
#                       |- plugin1
#                           |- translation
#                           |   |- ${TRANSLATIONS_NAME}_zh_CN.ts
#                           |   |- ${TRANSLATIONS_NAME}_zh_TW.ts

include (CMakeParseArguments)

function(GENERATED_QT_TRANSLATIONS)
    cmake_parse_arguments(PARA "ALL"
        "NAME;TARGET;TSDIR;QM_INSTALL_DIR;OUT_QRC;OUT_QRC_NAME;RESOUCE_PREFIX;INSTALL_COMPONENT"
        "SOURCES" ${ARGN})

    SET(TRANSLATIONS_NAME ${PROJECT_NAME})
    if(DEFINED PARA_NAME)
        SET(TRANSLATIONS_NAME ${PARA_NAME})
    endif()

    if(NOT DEFINED PARA_TARGET)
        set(PARA_TARGET ${PROJECT_NAME})
    endif()

    message("TRANSLATIONS_NAME:${TRANSLATIONS_NAME}; TARGET:${PARA_TARGET}; CURRENT_SOURCE_DIR:${CMAKE_SOURCE_DIR}")

    set(TS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Translations)
    if(DEFINED PARA_TSDIR)
        set(TS_DIR ${PARA_TSDIR})
    endif()
    if(NOT IS_DIRECTORY ${TS_DIR})
        file(MAKE_DIRECTORY ${TS_DIR})
    endif()
    SET(TS_FILES
        ${TS_DIR}/${TRANSLATIONS_NAME}_ar.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_ca.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_cs.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_da.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_de.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_el.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_en.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_en_GB.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_es.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_et.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_fi.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_fr.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_gd.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_gl.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_hu.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_it.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_ko.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_ja.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_nb.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_nn.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_ne.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_nl.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_oc.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_pl.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_pt_BR.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_pt_PT.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_ro.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_ru.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_sk.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_sl.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_sv.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_th.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_tr.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_uk.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_zh_CN.ts
        ${TS_DIR}/${TRANSLATIONS_NAME}_zh_TW.ts
        )

    if(NOT DEFINED QT_VERSION_MAJOR)
        find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
        if(NOT QT_FOUND)
            message(AUTHOR_WARNING "Don't found qt")
            set(QT_VERSION_MAJOR 5)
        endif()
    endif()
    OPTION(OPTION_TRANSLATIONS "Refresh translations on compile" ON)
    MESSAGE("Refresh translations on compile: ${OPTION_TRANSLATIONS}\n")
    IF(OPTION_TRANSLATIONS)
        FIND_PACKAGE(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS LinguistTools) #语言工具
        IF(NOT Qt${QT_VERSION_MAJOR}LinguistTools_FOUND)
            MESSAGE(WARNING "Could not find LinguistTools. Your build won't contain translations.")
        ELSE()
            
            LIST(APPEND SOURCE_FILES ${SOURCE_UI_FILES})
            if(PARA_SOURCES)
                SET(SOURCE_FILES ${PARA_SOURCES})
            endif()

            OPTION(ENABLE_UPDATE_TRANSLATIONS "Use Qt${QT_VERSION_MAJOR}_create_translation. Note: If set to ON, then make clean or rebuild will delete all .ts files" OFF)
            if(ENABLE_UPDATE_TRANSLATIONS)
                if(TARGET translations_update_${PARA_TARGET})
                    message(WARNING "translations_update_${PARA_TARGET} is existed")
                    return()
                endif()

                if(QT_VERSION_MAJOR EQUAL 6)
                    qt6_create_translation(QM_FILES_UPDATE ${SOURCE_FILES} ${TS_FILES}) # 生成或更新翻译源文件（.ts）和生成翻译文件（.qm） 文件
                elseif(QT_VERSION_MAJOR EQUAL 5)
                    #注：根据 https://bugreports.qt.io/browse/QTBUG-41736 ，qt5_create_translation这个宏会在make clean或rebuild时把全部ts文件都删掉后再重新生成，这意味着已经翻译好的文本会全部丢失，已有的解决方法也已经失效，而Qt官方也没有针对这个问题进行修复，因此不建议再使用这个宏了，还是手动生成ts文件再搭配qt5_add_translation比较保险。
                    qt5_create_translation(QM_FILES_UPDATE ${SOURCE_FILES} ${TS_FILES}) # 生成或更新翻译源文件（.ts）和生成翻译文件（.qm） 文件
                else()
                    qt_create_translation(QM_FILES_UPDATE ${SOURCE_FILES} ${TS_FILES}) # 生成或更新翻译源文件（.ts）和生成翻译文件（.qm） 文件
                endif()

                # 手动执行目标，生成或更新翻译源文件(.ts)
                ADD_CUSTOM_TARGET(translations_update_${PARA_TARGET} DEPENDS ${QM_FILES_UPDATE})
            endif()

            if(TARGET translations_${PARA_TARGET})
                message(WARNING "translations_${PARA_TARGET} is existed")
                return()
            endif()

            if(QT_VERSION_MAJOR EQUAL 6)
                qt6_add_translation(QM_FILES ${TS_FILES}) #生成翻译文件（.qm）
            elseif(QT_VERSION_MAJOR EQUAL 5)
                qt5_add_translation(QM_FILES ${TS_FILES}) #生成翻译文件（.qm）
            else()
                qt_add_translation(QM_FILES ${TS_FILES}) #生成翻译文件（.qm）
            endif()

            # 自动执行目标，生成翻译文件(.qm)
            if(PARA_ALL)
                ADD_CUSTOM_TARGET(translations_${PARA_TARGET} ALL DEPENDS ${QM_FILES})
            else()
                ADD_CUSTOM_TARGET(translations_${PARA_TARGET} DEPENDS ${QM_FILES})
            endif()
            if(ENABLE_UPDATE_TRANSLATIONS)
                add_dependencies(translations_${PARA_TARGET} translations_update_${PARA_TARGET})
            endif()

            if(NOT PARA_QM_INSTALL_DIR)
                set(PARA_QM_INSTALL_DIR translations)
            endif()

            set(RESOUCE_PREFIX "/${PARA_QM_INSTALL_DIR}")

            # 生成资源文件
            set(RESOURCE_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/translations_${TRANSLATIONS_NAME}.qrc")
            file(WRITE "${RESOURCE_FILE_NAME}"
                "<!DOCTYPE RCC>
                <RCC version=\"1.0\">
                <qresource prefix=\"${RESOUCE_PREFIX}\">
                ")
            foreach(qm ${QM_FILES})
                get_filename_component(qm_name ${qm} NAME)
                file(APPEND "${RESOURCE_FILE_NAME}"
                    "    <file alias=\"${qm_name}\">${qm}</file>\n")
            endforeach(qm)
            file(APPEND "${RESOURCE_FILE_NAME}"
                "  </qresource>
                </RCC>
                ")
            set(TRANSLATIONS_RESOURCE_FILES ${RESOURCE_FILE_NAME} PARENT_SCOPE)
            if(DEFINED PARA_OUT_QRC)
                set(${PARA_OUT_QRC} ${${PARA_OUT_QRC}} ${RESOURCE_FILE_NAME} PARENT_SCOPE)
            endif()
            if(DEFINED PARA_OUT_QRC_NAME)
                get_filename_component(OUT_QRC_NAME ${RESOURCE_FILE_NAME} NAME)
                set(${PARA_OUT_QRC_NAME} ${OUT_QRC_NAME} PARENT_SCOPE)
            endif()

            if(NOT DEFINED INSTALL_COMPONENT)
                set(PARA_INSTALL_COMPONENT Runtime)
            endif()
#            INSTALL_FILE(TARGET translations_${PARA_TARGET}
#                SOURCES ${QM_FILES}
#                DESTINATION "${PARA_QM_INSTALL_DIR}"
#                    COMPONENT ${PARA_INSTALL_COMPONENT}
#                )
            
            install(FILES ${QM_FILES} DESTINATION "${PARA_QM_INSTALL_DIR}"
                COMPONENT ${PARA_INSTALL_COMPONENT})
            # Copy to build tree for develop
            if(INSTALL_TO_BUILD_PATH OR ANDROID)
                if(ANDROID)
                    set(_QM_INSTALL_DIR ${CMAKE_BINARY_DIR}/assets/${PARA_QM_INSTALL_DIR})
                else()
                    set(_QM_INSTALL_DIR ${CMAKE_BINARY_DIR}/${PARA_QM_INSTALL_DIR})
                endif()
                add_custom_command(
                    TARGET translations_${PARA_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${_QM_INSTALL_DIR}"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QM_FILES} "${_QM_INSTALL_DIR}"
                    COMMENT "Copy qm file ${QM_FILES} to ${_QM_INSTALL_DIR}"
                    COMMAND_EXPAND_LISTS
                    VERBATIM)
            endif()

        ENDIF()
    ENDIF(OPTION_TRANSLATIONS)

endfunction()
