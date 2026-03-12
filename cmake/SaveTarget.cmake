
# 定义一个全局属性来存储所有保存的目标
define_property(GLOBAL PROPERTY GLOBAL_SAVE_TARGETS
    BRIEF_DOCS "List of saved targets"
    FULL_DOCS "List of all targets saved by SAVE_TARGET() function"
)

#===============================================================================
# SAVE_TARGET - 保存目标
#
# 用法：
#   SAVE_TARGET(TARGETS target1 target2 ...
#
# 参数说明：
#   TARGETS: 要保存的目标名称（必选）
#===============================================================================
function(SAVE_TARGET)
    # 解析参数
    set(options )
    set(oneValueArgs )
    set(multiValueArgs TARGETS)

    cmake_parse_arguments(PARA
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # 检查必需参数
    if(NOT PARA_TARGETS)
        message(FATAL_ERROR "SAVE_TARGET: TARGETS parameter is required")
    endif()

    foreach(target_name ${PARA_TARGETS})
        # 检查目标是否存在
        if(NOT TARGET ${target_name})
            message(WARNING "SAVE_TARGET: Target '${target_name}' does not exist")
            continue()
        endif()

        message(VERBOSE "Saving target: ${target_name}")

        get_property(saved_targets GLOBAL PROPERTY GLOBAL_SAVE_TARGETS)
        if(NOT target_name IN_LIST saved_targets)
            set_property(GLOBAL APPEND PROPERTY GLOBAL_SAVE_TARGETS ${target_name})
        endif()

        message(VERBOSE "  Save to global targets")
    endforeach()

endfunction()

#===============================================================================
# split_target - 分离目标名称为命名空间和组件
# 用法：split_target(<target_name> <namespace_var> <component_var>)
# 示例：
#   split_target("Qt6::Core" ns comp)  # ns=Qt6, comp=Core
#   split_target("OpenSSL::SSL" ns comp)  # ns=OpenSSL, comp=SSL
#   split_target("CURL::libcurl" ns comp)  # ns=CURL, comp=libcurl
#   split_target("RabbitCommon" ns comp)  # ns=RabbitCommon, comp=
#===============================================================================
function(split_target target_name namespace_var component_var)
    if(target_name MATCHES "^([^:]+)::(.+)$")
        # 匹配到命名空间::组件模式
        set(${namespace_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        set(${component_var} "${CMAKE_MATCH_2}" PARENT_SCOPE)
    else()
        # 没有组件，整个作为命名空间
        set(${namespace_var} "${target_name}" PARENT_SCOPE)
        set(${component_var} "" PARENT_SCOPE)
    endif()
endfunction()

#===============================================================================
# process_target_list - 处理目标列表，分离命名空间和组件
# 用法：process_target_list(<input_list>
#                          <output_namespaces_var>
#                          <output_components_var>
#                          [<output_original_var>])
#===============================================================================
function(process_target_list input_list output_namespaces_var output_components_var)
    set(namespaces "")
    set(components "")
    set(original_targets "")

    foreach(target IN LISTS input_list)
        list(APPEND original_targets "${target}")

        if(target MATCHES "^([^:]+)::(.+)$")
            # 有命名空间的目标
            list(APPEND namespaces "${CMAKE_MATCH_1}")
            list(APPEND components "${CMAKE_MATCH_2}")
        else()
            # 无命名空间的目标
            list(APPEND namespaces "${target}")
            list(APPEND components "")
        endif()
    endforeach()

    set(${output_namespaces_var} "${namespaces}" PARENT_SCOPE)
    set(${output_components_var} "${components}" PARENT_SCOPE)

    # 如果提供了第三个输出变量，也返回原始目标
    if(ARGC GREATER 3)
        set(${ARGV3} "${original_targets}" PARENT_SCOPE)
    endif()

    # 调试输出
    message(VERBOSE "Processed ${len} targets")
    foreach(i RANGE 0 ${len})
        if(i LESS len)
            list(GET original_targets ${i} orig)
            list(GET namespaces ${i} ns)
            list(GET components ${i} comp)
            message(VERBOSE "  [${i}] ${orig} -> ns='${ns}', comp='${comp}'")
        endif()
    endforeach()
endfunction()

#===============================================================================
# safe_find_dependency - 安全地查找依赖，处理带命名空间的目标
# 返回值：
#   ${target}_FOUND - 目标是否找到
#   ${target}_FOUND_COMPONENTS - 找到的组件列表
#===============================================================================
function(safe_find_dependency target)
    # 初始化返回值变量（在父作用域）
    set(${target}_FOUND FALSE PARENT_SCOPE)
    set(${target}_FOUND_COMPONENTS "" PARENT_SCOPE)

    split_target(${target} ns component)
    message(STATUS "Processing ${target} -> namespace: '${ns}', component: '${component}'")

    # 保存当前的 CMAKE_FIND_PACKAGE_NAME，以便恢复
    set(OLD_CMAKE_FIND_PACKAGE_NAME ${CMAKE_FIND_PACKAGE_NAME})

    if(ns AND component)
        # 有命名空间和组件
        if(component STREQUAL ns)
            # 特殊情况：命名空间和组件相同（如 cmark::cmark）
            message(STATUS "  Component equals namespace, using find_dependency(${ns})")
            find_dependency(${ns})

            # 检查是否找到
            if(${ns}_FOUND)
                set(${target}_FOUND TRUE PARENT_SCOPE)
                set(${target}_FOUND_COMPONENTS "${ns}" PARENT_SCOPE)
                message(STATUS "  √ Found ${target} (via ${ns})")
            else()
                set(${target}_FOUND FALSE PARENT_SCOPE)
                message(STATUS "  X Failed to find ${target}")
            endif()
        else()
            # 正常情况：命名空间和组件不同（如 Qt6::Core, OpenSSL::SSL）
            message(STATUS "  Using find_dependency(${ns} COMPONENTS ${component})")

            # 保存当前的组件列表
            set(OLD_CMAKE_FIND_PACKAGE_COMPONENTS ${CMAKE_FIND_PACKAGE_COMPONENTS})

            # 设置当前查找的组件
            set(CMAKE_FIND_PACKAGE_COMPONENTS ${component})

            find_dependency(${ns} COMPONENTS ${component})

            # 检查组件是否找到
            if(${ns}_${component}_FOUND OR ${ns}_FOUND)
                set(${target}_FOUND TRUE PARENT_SCOPE)
                set(${target}_FOUND_COMPONENTS "${component}" PARENT_SCOPE)
                message(STATUS "  √ Found ${target} (component ${component} of ${ns})")
            else()
                set(${target}_FOUND FALSE PARENT_SCOPE)
                message(STATUS "  X Failed to find ${target}")
            endif()

            # 恢复组件列表
            set(CMAKE_FIND_PACKAGE_COMPONENTS ${OLD_CMAKE_FIND_PACKAGE_COMPONENTS})
        endif()
    elseif(ns)
        # 只有命名空间（不太可能发生）
        message(STATUS "  Only namespace, using find_dependency(${ns})")
        find_dependency(${ns})

        if(${ns}_FOUND)
            set(${target}_FOUND TRUE PARENT_SCOPE)
            set(${target}_FOUND_COMPONENTS "${ns}" PARENT_SCOPE)
            message(STATUS "  √ Found ${target} (as ${ns})")
        else()
            set(${target}_FOUND FALSE PARENT_SCOPE)
            message(STATUS "  X Failed to find ${target}")
        endif()
    elseif(component)
        # 只有组件（无命名空间的目标）
        message(STATUS "  Only component, using find_dependency(${component})")
        find_dependency(${component})

        if(${component}_FOUND)
            set(${target}_FOUND TRUE PARENT_SCOPE)
            set(${target}_FOUND_COMPONENTS "${component}" PARENT_SCOPE)
            message(STATUS "  √ Found ${target} (as ${component})")
        else()
            set(${target}_FOUND FALSE PARENT_SCOPE)
            message(STATUS "  X Failed to find ${target}")
        endif()
    else()
        # 空目标
        message(WARNING "Empty target passed to safe_find_dependency")
        set(${target}_FOUND FALSE PARENT_SCOPE)
    endif()

    # 恢复 CMAKE_FIND_PACKAGE_NAME
    set(CMAKE_FIND_PACKAGE_NAME ${OLD_CMAKE_FIND_PACKAGE_NAME})

    # 返回找到的状态（也可以通过返回值返回）
    if(${target}_FOUND)
        return(TRUE)
    else()
        return(FALSE)
    endif()
endfunction()

#===============================================================================
# safe_find_dependency_ext - 增强版，带详细错误信息
# 返回值：
#   ${target}_FOUND - 目标是否找到
#   ${target}_FOUND_COMPONENTS - 找到的组件列表
#   ${target}_ERROR_REASON - 失败原因
#===============================================================================
function(safe_find_dependency_ext target)
    # 初始化返回值变量
    set(${target}_FOUND FALSE PARENT_SCOPE)
    set(${target}_FOUND_COMPONENTS "" PARENT_SCOPE)
    set(${target}_ERROR_REASON "" PARENT_SCOPE)

    split_target(${target} ns component)
    message(STATUS "Processing ${target} -> namespace: '${ns}', component: '${component}'")

    # 保存旧变量
    set(OLD_CMAKE_FIND_PACKAGE_NAME ${CMAKE_FIND_PACKAGE_NAME})
    set(OLD_CMAKE_FIND_PACKAGE_COMPONENTS ${CMAKE_FIND_PACKAGE_COMPONENTS})

    # 错误处理宏
    macro(handle_find_error package error_msg)
        set(${target}_FOUND FALSE PARENT_SCOPE)
        set(${target}_ERROR_REASON "Failed to find ${package}: ${error_msg}" PARENT_SCOPE)
        message(STATUS "  X ${${target}_ERROR_REASON}")
    endmacro()

    # 成功处理宏
    macro(handle_find_success package components)
        set(${target}_FOUND TRUE PARENT_SCOPE)
        set(${target}_FOUND_COMPONENTS "${components}" PARENT_SCOPE)
        set(${target}_ERROR_REASON "" PARENT_SCOPE)
        message(STATUS "  √ Found ${target}")
    endmacro()

    if(ns AND component)
        # 有命名空间和组件
        if(component STREQUAL ns)
            # 特殊情况：命名空间和组件相同
            message(STATUS "  Component equals namespace, using find_dependency(${ns})")

            # 尝试查找
            find_dependency(${ns})

            if(${ns}_FOUND)
                handle_find_success("${ns}" "${ns}")
            else()
                handle_find_error("${ns}" "Package ${ns} not found")
            endif()
        else()
            # 正常情况：命名空间和组件不同
            message(STATUS "  Using find_dependency(${ns} COMPONENTS ${component})")

            # 设置当前查找的组件
            set(CMAKE_FIND_PACKAGE_COMPONENTS ${component})

            # 尝试查找
            find_dependency(${ns} COMPONENTS ${component})

            # 检查组件是否找到
            if(${ns}_${component}_FOUND)
                handle_find_success("${ns}" "${component}")
            elseif(${ns}_FOUND)
                # 包找到了但组件没找到
                set(found_components "")
                if(DEFINED ${ns}_FOUND_COMPONENTS)
                    set(found_components " (found components: ${${ns}_FOUND_COMPONENTS})")
                endif()
                handle_find_error("${ns}" "Package ${ns} found but component ${component} not found${found_components}")
            else()
                handle_find_error("${ns}" "Package ${ns} not found")
            endif()
        endif()
    elseif(ns)
        # 只有命名空间
        message(STATUS "  Only namespace, using find_dependency(${ns})")
        find_dependency(${ns})

        if(${ns}_FOUND)
            handle_find_success("${ns}" "${ns}")
        else()
            handle_find_error("${ns}" "Package ${ns} not found")
        endif()
    elseif(component)
        # 只有组件
        message(STATUS "  Only component, using find_dependency(${component})")
        find_dependency(${component})

        if(${component}_FOUND)
            handle_find_success("${component}" "${component}")
        else()
            handle_find_error("${component}" "Package ${component} not found")
        endif()
    else()
        # 空目标
        handle_find_error("" "Empty target passed to safe_find_dependency_ext")
    endif()

    # 恢复变量
    set(CMAKE_FIND_PACKAGE_NAME ${OLD_CMAKE_FIND_PACKAGE_NAME})
    set(CMAKE_FIND_PACKAGE_COMPONENTS ${OLD_CMAKE_FIND_PACKAGE_COMPONENTS})

    # 返回找到的状态
    if(${target}_FOUND)
        return(TRUE)
    else()
        return(FALSE)
    endif()
endfunction()

#===============================================================================
# add_saved_target_to_android_extra_libs - 将保存的目标添加到Android额外库
# 
# 用法：
#   add_saved_target_to_android_extra_libs(TARGET <main_target> 
#                                          LIB_TARGETS <lib1> [lib2 ...])
#===============================================================================
function(add_saved_target_to_android_extra_libs)
    set(oneValueArgs TARGET)
    set(multiValueArgs LIB_TARGETS)
    
    cmake_parse_arguments(ANDROID_LIBS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT ANDROID_LIBS_TARGET)
        message(FATAL_ERROR "TARGET parameter is required")
    endif()
    
    if(NOT ANDROID_LIBS_LIB_TARGETS)
        message(FATAL_ERROR "LIB_TARGETS parameter is required")
    endif()
    
    set(extra_libs "")
    
    foreach(lib_target ${ANDROID_LIBS_LIB_TARGETS})
        if(TARGET ${lib_target})
            list(APPEND extra_libs "$<TARGET_FILE:${lib_target}>")
            message(STATUS "Adding ${lib_target} to QT_ANDROID_EXTRA_LIBS")
        else()
            message(WARNING "Target ${lib_target} does not exist, skipping")
        endif()
    endforeach()
    
    if(extra_libs)
        # 获取现有的QT_ANDROID_EXTRA_LIBS
        get_target_property(current_extra_libs ${ANDROID_LIBS_TARGET} QT_ANDROID_EXTRA_LIBS)
        
        if(current_extra_libs)
            set(extra_libs "${current_extra_libs};${extra_libs}")
        endif()
        
        set_target_properties(${ANDROID_LIBS_TARGET} PROPERTIES
            QT_ANDROID_EXTRA_LIBS "${extra_libs}"
        )
        
        # 同时更新全局属性
        set_property(GLOBAL APPEND GLOBAL_QT_ANDROID_EXTRA_LIBS "${extra_libs}")
        
        message(STATUS "QT_ANDROID_EXTRA_LIBS for ${ANDROID_LIBS_TARGET}: ${extra_libs}")
    endif()
    
endfunction()
