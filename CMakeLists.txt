cmake_minimum_required(VERSION 3.0.0)
project(TypeScriptExample VERSION 0.1.0)

add_subdirectory(Dependencies)

add_executable(TypeScriptExample main.cpp)

target_link_libraries(TypeScriptExample
    PRIVATE AppRuntime
    PRIVATE NativeEngine
    PRIVATE NativeInput
    PRIVATE NativeOptimizations
    PRIVATE Console
    PRIVATE Window
    PRIVATE ScriptLoader
    PRIVATE XMLHttpRequest
    PRIVATE Canvas
    PRIVATE glfw
    PRIVATE GraphicsDevice)

target_compile_features(TypeScriptExample PRIVATE cxx_std_17)

# Set bundle.js to be copied to the build folder.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/TypeScript/build/bundle.js")
    set(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/TypeScript/build/bundle.js")
    target_sources(TypeScriptExample PRIVATE ${SCRIPT})

    get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)

    add_custom_command(
        OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
        COMMENT "Copying ${SCRIPT_NAME}"
        MAIN_DEPENDENCY "${SCRIPT}")
    
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPT})
else()
    message(FATAL_ERROR "Javascript bundle is required. Please run 'npm run build' from the TypeScript folder.")
endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TypeScriptExample)

if(MSVC)
    target_compile_definitions(TypeScriptExample PRIVATE TARGET_PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(TypeScriptExample PRIVATE TARGET_PLATFORM_OSX)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    target_link_libraries(TypeScriptExample PRIVATE ${JSCORE_LIBRARY})
    target_link_libraries(TypeScriptExample PRIVATE "-framework Cocoa" "-framework MetalKit" "-framework QuartzCore" "-framework CoreFoundation" "-framework CoreGraphics")
else()
    target_compile_definitions(TypeScriptExample PRIVATE TARGET_PLATFORM_LINUX)
endif()