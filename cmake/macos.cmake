# target_add_resources: Helper function to add a specific resource to a bundle
function(target_add_resources target base resources)
  message("Add resource '${resources}' to target '${target}' at destination '${base}' ...")
  if(IS_ABSOLUTE ${base})
    message(FATAL_ERROR "${base} must be relative path")
  endif()
  if(CMAKE_MACOSX_BUNDLE)
    foreach(resource IN LISTS resources)
      target_sources(${target} PRIVATE "${resource}")
      set_property(SOURCE "${resource}" PROPERTY MACOSX_PACKAGE_LOCATION Resources/${base})
      source_group("Resources" FILES "${resource}")
    endforeach()
  else()
    INSTALL(FILES ${resources}
      DESTINATION ${base}
        COMPONENT Application)
  endif()
endfunction()

# target_install_resources: Helper function to add resources into bundle
function(target_install_resources target resource)
  message("Installing resources '${resource}' for target '${target}' ...")
  if(NOT CMAKE_MACOSX_BUNDLE)
    return()
  endif()
  if(EXISTS "${resource}")
    cmake_path(GET resource PARENT_PATH relative_path)
    cmake_path(
      RELATIVE_PATH
      resource
      BASE_DIRECTORY "${relative_path}/"
      OUTPUT_VARIABLE base_path
    )
    file(GLOB_RECURSE data_files "${resource}/*")
    foreach(data_file IN LISTS data_files)
      cmake_path(
        RELATIVE_PATH
        data_file
        BASE_DIRECTORY "${resource}/"
        OUTPUT_VARIABLE relative_path
      )
      cmake_path(GET relative_path PARENT_PATH relative_path)
      set(relative_path ${base_path}/${relative_path})
      target_add_resources(${target} ${relative_path} "${data_file}")
      #target_sources(${target} PRIVATE "${data_file}")
      #set_property(SOURCE "${data_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relative_path}")
      #source_group("Resources/${relative_path}" FILES "${data_file}")
      #message("Set source: ${data_file} to Resources/${relative_path}")
    endforeach()
  endif()
endfunction()

# _handle_generator_expression_dependency: Helper function to yield dependency from a generator expression
function(_handle_generator_expression_dependency library)
  set(oneValueArgs FOUND_VAR)
  set(multiValueArgs)
  cmake_parse_arguments(var "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(${var_FOUND_VAR} "${var_FOUND_VAR}-NOTFOUND")

  message(DEBUG "Checking ${library}...")

  if(library MATCHES "\\$<\\$<PLATFORM_ID:[^>]+>:.+>" OR library MATCHES "\\$<\\$<NOT:\\$<PLATFORM_ID:[^>]+>>:.+>")
    # Platform-dependent generator expression found. Platforms are a comma-separated list of CMake host OS identifiers.
    # Convert to CMake list and check if current host OS is contained in list.
    string(REGEX REPLACE "\\$<.*\\$<PLATFORM_ID:([^>]+)>>?:([^>]+)>" "\\1;\\2" gen_expression "${library}")
    list(GET gen_expression 0 gen_platform)
    list(GET gen_expression 1 gen_library)
    string(REPLACE "," ";" gen_platform "${gen_platform}")

    set(${var_FOUND_VAR} "${var_FOUND_VAR}-SKIP")

    if(library MATCHES "\\$<\\$<NOT:.+>.+>")
      if(NOT CMAKE_SYSTEM_NAME IN_LIST gen_platform)
        set(${var_FOUND_VAR} "${gen_library}")
      endif()
    else()
      if(CMAKE_SYSTEM_NAME IN_LIST gen_platform)
        set(${var_FOUND_VAR} "${gen_library}")
      endif()
    endif()
  elseif(library MATCHES "\\$<\\$<BOOL:[^>]+>:.+>")
    # Boolean generator expression found. Consider parameter a CMake variable that resolves into a CMake-like boolean
    # value for a simple conditional check.
    string(REGEX REPLACE "\\$<\\$<BOOL:([^>]+)>:([^>]+)>" "\\1;\\2" gen_expression "${library}")
    list(GET gen_expression 0 gen_boolean)
    list(GET gen_expression 1 gen_library)

    set(${var_FOUND_VAR} "${var_FOUND_VAR}-SKIP")

    if(${gen_boolean})
      set(${var_FOUND_VAR} "${gen_library}")
    endif()
  elseif(library MATCHES "\\$<TARGET_NAME_IF_EXISTS:[^>]+>")
    # Target-dependent generator expression found. Consider parameter to be a CMake target identifier and check for
    # target existence.
    string(REGEX REPLACE "\\$<TARGET_NAME_IF_EXISTS:([^>]+)>" "\\1" gen_target "${library}")

    set(${var_FOUND_VAR} "${var_FOUND_VAR}-SKIP")

    if(TARGET ${gen_target})
      set(${var_FOUND_VAR} "${gen_target}")
    endif()
  elseif(library MATCHES "\\$<.*Qt6::(EntryPointPrivate|QDarwin.*PermissionPlugin)>")
    set(${var_FOUND_VAR} "${var_FOUND_VAR}-SKIP")
  else()
    # Unknown or unimplemented generator expression found. Abort script run to either add to ignore list or implement
    # detection.
    message(FATAL_ERROR "${library} is an unsupported generator expression for linked libraries.")
  endif()

  if(CMAKE_VERSION VERSION_LESS 3.25)
    set(${var_FOUND_VAR} ${var_FOUND_VAR} PARENT_SCOPE)
  else()
    return(PROPAGATE ${var_FOUND_VAR})
  endif()
endfunction()

# find_dependencies: Check linked interface and direct dependencies of target
function(find_dependencies)
  set(oneValueArgs TARGET FOUND_VAR)
  set(multiValueArgs)
  cmake_parse_arguments(var "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT DEFINED is_root)
    # Root of recursive dependency resolution
    set(is_root TRUE)
    set(nested_depth 0)
  else()
    # Branch of recursive dependency resolution
    set(is_root FALSE)
    math(EXPR nested_depth "${nested_depth}+1")
  endif()

  # * LINK_LIBRARIES are direct dependencies
  # * INTERFACE_LINK_LIBRARIES are transitive dependencies
  get_target_property(linked_libraries ${var_TARGET} LINK_LIBRARIES)
  get_target_property(interface_libraries ${var_TARGET} INTERFACE_LINK_LIBRARIES)
  message(DEBUG "[${nested_depth}] Linked libraries in target ${var_TARGET}: ${linked_libraries}")
  message(DEBUG "[${nested_depth}] Linked interface libraries in target ${var_TARGET}: ${interface_libraries}")

  # Consider CMake targets only
  list(FILTER linked_libraries INCLUDE REGEX ".+::.+")
  list(FILTER interface_libraries INCLUDE REGEX ".+::.+")

  foreach(library IN LISTS linked_libraries interface_libraries)
    if(NOT library)
      continue()
    elseif(library MATCHES "\\$<.*:[^>]+>")
      # Generator expression found
      _handle_generator_expression_dependency(${library} FOUND_VAR found_library)
      if(found_library STREQUAL found_library-SKIP)
        continue()
      elseif(found_library)
        set(library ${found_library})
      endif()
    endif()

    message(DEBUG "[${nested_depth}] Found ${library}...")

    if(NOT library IN_LIST ${var_FOUND_VAR})
      list(APPEND found_libraries ${library})
      # Enter recursive branch
      find_dependencies(TARGET ${library} FOUND_VAR ${var_FOUND_VAR})
    endif()
  endforeach()

  if(NOT is_root)
    # Exit recursive branch
    return(PROPAGATE found_libraries)
  endif()

  list(REMOVE_DUPLICATES found_libraries)
  list(APPEND ${var_FOUND_VAR} ${found_libraries})

  return(PROPAGATE ${var_FOUND_VAR})
endfunction()

# find_qt_plugins: Find and add Qt plugin libraries associated with Qt component to target
function(find_qt_plugins)
  set(oneValueArgs COMPONENT TARGET FOUND_VAR)
  cmake_parse_arguments(var "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  string(REPLACE "::" ";" library_tuple "${var_COMPONENT}")
  list(GET library_tuple 0 library_namespace)
  list(GET library_tuple 1 library_name)

  if(NOT ${library_namespace} MATCHES "Qt6?")
    message(FATAL_ERROR "'find_qt_plugins' has to be called with a valid target from the Qt or Qt6 namespace.")
  endif()

  list(
    APPEND
    qt_plugins_Core
    platforms
    printsupport
    styles
    imageformats
    iconengines
  )
  list(APPEND qt_plugins_Gui platforminputcontexts)
  list(APPEND qt_plugins_Sql sqldrivers)
  list(APPEND qt_plugins_3dRender sceneparsers geometryloaders)
  list(APPEND qt_plugins_3dQuickRender renderplugins)
  list(APPEND qt_plugins_Positioning position)
  list(APPEND qt_plugins_Location geoservices)
  list(APPEND qt_plugins_TextToSpeech texttospeech)
  list(APPEND qt_plugins_WebView webview)

  if(qt_plugins_${library_name})
    get_target_property(library_location ${var_COMPONENT} IMPORTED_LOCATION)
    get_target_property(is_framework ${var_COMPONENT} FRAMEWORK)

    if(is_framework)
      # Resolve Qt plugin location relative to framework binary location on macOS
      set(plugins_location "../../../../../plugins")
      cmake_path(ABSOLUTE_PATH plugins_location BASE_DIRECTORY "${library_location}" NORMALIZE)
    else()
      # Resolve Qt plugin location relative to dynamic library location
      set(plugins_location "../../plugins")
      cmake_path(ABSOLUTE_PATH plugins_location BASE_DIRECTORY "${library_location}" NORMALIZE)
    endif()

    foreach(plugin IN ITEMS ${qt_plugins_${library_name}})
      if(NOT plugin IN_LIST plugins_list)
        if(EXISTS "${plugins_location}/${plugin}")
          # Gather all .dll or .dylib files in given plugin subdirectory
          file(
            GLOB plugin_libraries
            RELATIVE "${plugins_location}/${plugin}"
            "${plugins_location}/${plugin}/*.dylib"
            "${plugins_location}/${plugin}/*.dll"
          )
          message(DEBUG "Found Qt plugin ${plugin} libraries: ${plugin_libraries}")
          foreach(plugin_library IN ITEMS ${plugin_libraries})
            set(plugin_full_path "${plugins_location}/${plugin}/${plugin_library}")
            list(APPEND plugins_list ${plugin_full_path})
          endforeach()
        endif()
      endif()
    endforeach()
  endif()

  set(${var_FOUND_VAR} ${plugins_list})
  return(PROPAGATE ${var_FOUND_VAR})
endfunction()

# _bundle_dependencies: Resolve 3rd party dependencies and add them to macOS app bundle
function(_bundle_dependencies target)
  message(DEBUG "Discover dependencies of target ${target}...")
  set(found_dependencies)
  find_dependencies(TARGET ${target} FOUND_VAR found_dependencies)

  get_property(obs_module_list GLOBAL PROPERTY OBS_MODULES_ENABLED)
  list(LENGTH obs_module_list num_modules)
  if(num_modules GREATER 0)
    add_dependencies(${target} ${obs_module_list})
    set_property(TARGET ${target} APPEND PROPERTY XCODE_EMBED_PLUGINS ${obs_module_list})
    foreach(module IN LISTS obs_module_list)
      find_dependencies(TARGET ${module} FOUND_VAR found_dependencies)
    endforeach()
  endif()

  list(REMOVE_DUPLICATES found_dependencies)

  set(library_paths)
  set(plugins_list)
  file(GLOB sdk_library_paths /Applications/Xcode*.app)
  set(system_library_path "/usr/lib/")

  foreach(library IN LISTS found_dependencies)
    get_target_property(library_type ${library} TYPE)
    get_target_property(is_framework ${library} FRAMEWORK)
    get_target_property(is_imported ${library} IMPORTED)

    if(is_imported)
      get_target_property(imported_location ${library} LOCATION)
      if(NOT imported_location)
        continue()
      endif()

      set(is_xcode_framework FALSE)
      set(is_system_framework FALSE)

      foreach(sdk_library_path IN LISTS sdk_library_paths)
        if(is_xcode_framework)
          break()
        endif()
        cmake_path(IS_PREFIX sdk_library_path "${imported_location}" is_xcode_framework)
      endforeach()
      cmake_path(IS_PREFIX system_library_path "${imported_location}" is_system_framework)

      if(is_system_framework OR is_xcode_framework)
        continue()
      elseif(is_framework)
        file(REAL_PATH "../../.." library_location BASE_DIRECTORY "${imported_location}")
      elseif(NOT library_type STREQUAL "STATIC_LIBRARY")
        if(NOT imported_location MATCHES ".+\\.a")
          set(library_location "${imported_location}")
        else()
          continue()
        endif()
      else()
        continue()
      endif()

      if(library MATCHES "Qt6?::.+")
        find_qt_plugins(COMPONENT ${library} TARGET ${target} FOUND_VAR plugins_list)
      endif()
      list(APPEND library_paths ${library_location})
    elseif(NOT is_imported AND library_type STREQUAL "SHARED_LIBRARY")
      list(APPEND library_paths ${library})
    endif()
  endforeach()

  list(REMOVE_DUPLICATES plugins_list)
  foreach(plugin IN LISTS plugins_list)
    cmake_path(GET plugin PARENT_PATH plugin_path)
    set(plugin_base_dir "${plugin_path}/../")
    cmake_path(SET plugin_stem_dir NORMALIZE "${plugin_base_dir}")
    cmake_path(RELATIVE_PATH plugin_path BASE_DIRECTORY "${plugin_stem_dir}" OUTPUT_VARIABLE plugin_file_name)
    target_sources(${target} PRIVATE "${plugin}")
    set_source_files_properties(
      "${plugin}"
      PROPERTIES MACOSX_PACKAGE_LOCATION "plugins/${plugin_file_name}" XCODE_FILE_ATTRIBUTES "CodeSignOnCopy"
    )
    source_group("Qt plugins" FILES "${plugin}")
  endforeach()

  list(REMOVE_DUPLICATES library_paths)
  set_property(TARGET ${target} APPEND PROPERTY XCODE_EMBED_FRAMEWORKS ${library_paths})
endfunction()
