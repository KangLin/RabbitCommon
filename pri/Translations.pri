# For autocompiling qm-files.
# qthelp://org.qt-project.qmake.5120/qmake/qmake-variable-reference.html

#
# Use:
# Include this file into the project file (.pro)
#    TRANSLATIONS_DIR =
#    TRANSLATIONS_NAME = 
#    include(../pri/Translations.pri) 

# Add translation resource in source code
#     Q_INIT_RESOURCE(translations_${TRANSLATIONS_NAME}); // Initial resource
#     QTranslator translator;
#     translator.load(CRabbitCommonGlobalDir::Instance()->GetDirTranslations()
#                   + "/" + qApp->applicationName() + "_" + QLocale::system().name() + ".qm");
#               //If is library，the qApp->applicationName() Replace with ${TRANSLATIONS_NAME}
#     qApp->installTranslator(&translator);
#
#     //After use, use the following to release resources
#     Q_CLEANUP_RESOURCE(translations_${TRANSLATIONS_NAME});//release resources
#
# debug translate resources as resource file embed programs

#
# Other system distribution mode, as a file in the installation directory of
# the program Translations directory
# Program installation directory:
#   AppRoot 
#       |- bin
#       |    |- App.exe
#       |- lib
#       |      
#       |- translations
#              |- ${TRANSLATIONS_NAME}_zh_CN.qm
#              |- ${TRANSLATIONS_NAME}_zh_TW.qm
#
#
# Android install directory:
#    assets
#       |- translations
#              |- ${TRANSLATIONS_NAME}_zh_CN.qm
#              |- ${TRANSLATIONS_NAME}_zh_TW.qm
#
#
# Source directory:
#   SourceRoot 
#          |- App
#          |   |- Resource
#          |         |-Translations
#          |               |- ${TRANSLATIONS_NAME}_zh_CN.ts
#          |               |- ${TRANSLATIONS_NAME}_zh_TW.ts
#          |- pri
#          |      |- Translations.pri
#          |      |- lrelease.pri
#          |- Src
#              |- Resource
#                   |-Translations
#                         |- ${TRANSLATIONS_NAME}_zh_CN.ts
#                         |- ${TRANSLATIONS_NAME}_zh_TW.ts


#
# 使用：
# 把此文件包含进入工程文件中(.pro)
#    TRANSLATIONS_DIR =
#    TRANSLATIONS_NAME = 
#    include(../pri/Translations.pri) 

# 在代码中加载翻译资源
#     Q_INIT_RESOURCE(translations_${TRANSLATIONS_NAME}); //初始化资源
#     QTranslator translator;
#     translator.load(CRabbitCommonGlobalDir::Instance()->GetDirTranslations()
#                   + "/" + qApp->applicationName() + "_" + QLocale::system().name() + ".qm");
#            //如果是库，qApp->applicationName() 用 ${TRANSLATIONS_NAME} 替换
#     qApp->installTranslator(&translator);
#
#     //使用完成后，用下面释放资源
#     Q_CLEANUP_RESOURCE(translations_${TRANSLATIONS_NAME});//释放资源
#
# debug 翻译资源做为资源文件嵌入程序
#
# 其它系统发行模式下，做为文件放在程序的安装目录 Translations 目录下
# 程序的安装目录：
#   AppRoot 
#       |- bin
#       |    |- App.exe
#       |- lib
#       |      
#       |- translations
#              |- ${TRANSLATIONS_NAME}_zh_CN.qm
#              |- ${TRANSLATIONS_NAME}_zh_TW.qm
#
#
# Android 翻译安装目录:
#    assets
#       |- translations
#              |- ${TRANSLATIONS_NAME}_zh_CN.qm
#              |- ${TRANSLATIONS_NAME}_zh_TW.qm
#
#
# 源码目录：
#   SourceRoot 
#          |- App
#          |   |- Resource
#          |         |-Translations
#          |               |- ${TRANSLATIONS_NAME}_zh_CN.ts
#          |               |- ${TRANSLATIONS_NAME}_zh_TW.ts
#          |- pri
#          |      |- Translations.pri
#          |      |- lrelease.pri
#          |- Src
#              |- Resource
#                   |-Translations
#                         |- ${TRANSLATIONS_NAME}_zh_CN.ts
#                         |- ${TRANSLATIONS_NAME}_zh_TW.ts

isEmpty(TRANSLATIONS_DIR) : TRANSLATIONS_DIR = $$_PRO_FILE_PWD_
isEmpty(TRANSLATIONS_NAME) : TRANSLATIONS_NAME = $${TARGET}
#message("TRANSLATIONS_NAME:$$TRANSLATIONS_NAME;TRANSLATIONS_DIR:$$TRANSLATIONS_DIR")
TRANSLATIONS_TS_FILES = \
    $$TRANSLATIONS_DIR/Resource/Translations/$${TRANSLATIONS_NAME}_zh_CN.ts \
    $$TRANSLATIONS_DIR/Resource/Translations/$${TRANSLATIONS_NAME}_zh_TW.ts
    
TRANSLATIONS *= $$TRANSLATIONS_TS_FILES

QM_FILES_RESOURCE_PREFIX = translations

#android : CONFIG *= embed_translations
isEmpty(QM_FILES_INSTALL_PATH){
    android: QM_FILES_INSTALL_PATH = $$PREFIX/assets/translations
    else: QM_FILES_INSTALL_PATH = $$PREFIX/translations
}

CONFIG(debug, debug|release) {
    CONFIG *= embed_translations
    DEFINES *= _DEBUG
}

#lessThan(QT_MAJOR_VERSION, 5) : LESS_5_12 = 1
#greaterThan(QT_MAJOR_VERSION, 4) : lessThan(QT_MINOR_VERSION, 12) LESS_5_12 = 1

#isEmpty(LESS_5_12) {
#    CONFIG *= lrelease
#    qm_files.CONFIG = no_check_exist
#} else {
    include($$PWD/lrelease.pri)
#}

TRANSLATIONS_NAME=
TRANSLATIONS_DIR=
