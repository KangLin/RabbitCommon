cmake_minimum_required(VERSION 3.5)

project(Tests LANGUAGES CXX)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

ADD_TARGET(NAME TestGenerateDefaultJsonFile
    ISEXE
    NO_TRANSLATION
    SOURCE_FILES TestGenerateDefaultJsonFile.cpp TestGenerateDefaultJsonFile.h
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
    )

add_test(NAME TestGenerateDefaultJsonFile
    COMMAND TestGenerateDefaultJsonFile
    WORKING_DIRECTORY ${OUTPUT_DIR}
    COMMAND_EXPAND_LISTS
    )

ADD_TARGET(NAME TestGenerateJsonFile
    ISEXE
    NO_TRANSLATION
    SOURCE_FILES TestGenerateJsonFile.cpp TestGenerateJsonFile.h
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
    )

add_test(NAME TestGenerateJsonFile
    COMMAND TestGenerateJsonFile -f "test.json" --pv "2.0.0" -i "info" -t "time" -s "ubuntu" -a "x86_64" -c "aab854ec7cc6831d591237e609a8bf36" -n "rabbitremotecontrol_0.0.26_amd64.deb" -p "package-file" -u "https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.26/rabbitremotecontrol_0.0.26_amd64.deb,https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.26/rabbitremotecontrol_0.0.26_amd64.deb/download" --home "home" -m "1.9.0"
    WORKING_DIRECTORY ${OUTPUT_DIR}
    COMMAND_EXPAND_LISTS
    )

ADD_TARGET(NAME UnitTests
    ISEXE
    NO_TRANSLATION
    SOURCE_FILES UnitTests.h UnitTests.cpp
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
    )

add_test(NAME UnitTests
    COMMAND UnitTests
    WORKING_DIRECTORY ${OUTPUT_DIR}
    COMMAND_EXPAND_LISTS
    )

ADD_TARGET(NAME TestUpdater
    ISEXE
    NO_TRANSLATION
    SOURCE_FILES TestUpdater.h TestUpdater.cpp
    PRIVATE_LIBS RabbitCommon ${QT_LIBRARIES}
    INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Src>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Src>"
    )

add_test(NAME TestUpdater
    COMMAND TestUpdater
    WORKING_DIRECTORY ${OUTPUT_DIR}
    COMMAND_EXPAND_LISTS
    )

set_tests_properties(TestUpdater PROPERTIES
  FAIL_REGULAR_EXPRESSION "\[[01][0-9]:[0-6][0-9]:[0-6][0-9]\.[0-9]{3} [0-9A-Fa-f]+\|[0-9A-Fa-f]+ E\] RabbitCommon.Updater"
)

if(NOT EXISTS ${OUTPUT_DIR}/data)
    file(COPY data DESTINATION ${OUTPUT_DIR})
endif()
