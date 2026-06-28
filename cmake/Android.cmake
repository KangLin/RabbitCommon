# Process android build file build.gradle
# See: https://doc.qt.io/qt-6/zh/qt-android-generate-deployment-settings.html
function(process_build_gradle name build_dir)

    # 创建临时 CMake 脚本来修改 build.gradle
    set(FIX_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/fix_build_gradle_${name}.cmake")

    file(WRITE "${FIX_SCRIPT}"
        "
        if(EXISTS \"\${BUILD_DIR}/build.gradle\")
            file(READ \"\${BUILD_DIR}/build.gradle\" CONTENT)

            # 方式1：添加中文支持
            #string(REPLACE \"resConfig \\\"en\\\"\" \"resConfig \\\"en\\\", \\\"zh\\\"\" CONTENT \"\${CONTENT}\")

            # 方式2：或者直接注释掉 resConfigs 行
            string(REGEX REPLACE \"resConfig \\\"en\\\"\" \"// resConfig \\\"en\\\"  // Disabled for multi-language\" CONTENT \"\${CONTENT}\")

            file(WRITE \"\${BUILD_DIR}/build.gradle\" \"\${CONTENT}\")
            message(STATUS \"Updated build.gradle for multi-language\")
        else()
            message(FATAL_ERROR \"\${BUILD_DIR}/build.gradle not found\")
        endif()
        "
    )

    # 创建一个独立的目标，在 androiddeployqt 之后运行
    add_custom_target(fix_build_gradle_${name} ALL
        COMMAND ${CMAKE_COMMAND}
            -DBUILD_DIR="${build_dir}"
            -P "${FIX_SCRIPT}"
        DEPENDS ${name}_make_apk  # 依赖 APK 生成目标
        COMMENT "Patching build.gradle after androiddeployqt"
    )
endfunction()
