project(RabbitCommonApp)

set(SOURCE_FILES
    main.cpp
    )

#翻译
include(${CMAKE_SOURCE_DIR}/cmake/Qt5CorePatches.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Translations.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(RESOURCE_FILES Resource/Resource.qrc
    "${TRANSLATIONS_RESOURCE_FILES}")

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
IF(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
ENDIF()
if(ANDROID)
    add_library(${PROJECT_NAME} ${SOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()
#add_dependencies(${PROJECT_NAME} translations_${TRANSLATIONS_NAME})
if(BUILD_ABOUT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DHAVE_ABOUT)
endif()
if(BUILD_UPDATE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DHAVE_UPDATE)
endif()
if(MINGW)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "-mwindows")
elseif(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE
        "/SUBSYSTEM:WINDOWS\",5.01\" /ENTRY:mainCRTStartup")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE RabbitCommon ${QT_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE
      "${CMAKE_BINARY_DIR}/Src" "${CMAKE_SOURCE_DIR}/Src" )
if(ANDROID)
    # Install target
    INSTALL(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION "libs/${ANDROID_ABI}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "libs/${ANDROID_ABI}"
            )
else()
    # Install target
    INSTALL(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}"
        )
endif()    
# Install other files
INSTALL(FILES ${OTHER_FILES} DESTINATION ".")

IF("Release" STREQUAL CMAKE_BUILD_TYPE AND WIN32)
    SET(RUNTIME_FILE "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
    IF(CMAKE_BUILD_TYPE MATCHES Release AND MINGW)
        #windeployqt 分发时，是根据是否 strip 来判断是否是 DEBUG 版本,而用mingw编译时,qt没有自动 strip
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND strip "$<TARGET_FILE:${PROJECT_NAME}>"
            )
    ENDIF()

    #注意 需要把 ${QT_INSTALL_DIR}/bin 加到环境变量PATH中  
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${QT_INSTALL_DIR}/bin/windeployqt"
        --compiler-runtime
        --verbose 7
        "$<TARGET_FILE:${PROJECT_NAME}>"
        )
    INSTALL(DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
        DESTINATION "${CMAKE_INSTALL_BINDIR}")
    
ENDIF()

IF(ANDROID)
    Set(JSON_FILE ${CMAKE_BINARY_DIR}/android_deployment_settings.json)
    GENERATED_DEPLOYMENT_SETTINGS(NAME ${JSON_FILE}
        APPLACTION "${CMAKE_BINARY_DIR}/bin/lib${PROJECT_NAME}.so")

    set(ANDROID_DELOP_PARA --gradle)
    if(Qt5_VERSION VERSION_LESS 5.8.0)
        find_program(ANT NAMES ant) # PATHS  "/usr/bin")
        MESSAGE("ant:${ANT}\n")
        if(ANT_FOUND)
            set(ANDROID_DELOP_PARA --ant ${ANT})
        endif()
    endif()

    add_custom_target(APK #TARGET ${PROJECT_NAME} POST_BUILD
        #COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" ${CMAKE_BINARY_DIR}/android-build/libs/${ANDROID_ABI}/$<TARGET_FILE_NAME:${PROJECT_NAME}>
        #COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/android-build/libs/${ANDROID_ABI}
        #COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>/*" ${CMAKE_BINARY_DIR}/android-build/libs/${ANDROID_ABI}
        #注意 需要把 ${QT_INSTALL_DIR}/bin 加到环境变量PATH中
        COMMAND "${QT_INSTALL_DIR}/bin/androiddeployqt"
            --output ${CMAKE_BINARY_DIR}/android-build
            --input ${JSON_FILE}
            --verbose
            --android-platform ${ANDROID_PLATFORM}
            ${ANDROID_DELOP_PARA}
            #--jdk $ENV{JAVA_HOME}
    )
ENDIF(ANDROID)
