cmake_minimum_required(VERSION 3.5)

project(Tests LANGUAGES CXX)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
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
    COMMAND TestGenerateJsonFile -f "test.json" --pv "2.0.0" -i "info" -t "time" -s "windows" -a "x86" -c "asbdasersdfasdfawer" -n "rabbitcommon" -p "package-file" -u "github.com" --home "home" -m "1.5.7"
    WORKING_DIRECTORY ${OUTPUT_DIR}
    COMMAND_EXPAND_LISTS
    )

if(NOT EXISTS ${OUTPUT_DIR}/data)
    file(COPY data DESTINATION ${OUTPUT_DIR})
endif()
