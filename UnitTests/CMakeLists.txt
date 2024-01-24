cmake_minimum_required(VERSION 3.5)

project(UnitTests LANGUAGES CXX)

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
    COMMAND TestGenerateJsonFile -f a.json --foc 1
    WORKING_DIRECTORY ${OUTPUT_DIR}
    )
