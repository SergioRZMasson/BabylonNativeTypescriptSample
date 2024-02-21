cmake_minimum_required(VERSION 3.0.0)
project(TypescriptExample VERSION 0.1.0)

add_subdirectory(Dependencies)

add_executable(TypescriptExample main.cpp)

target_link_libraries(TypescriptExample
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

target_compile_features(TypescriptExample PRIVATE cxx_std_17)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Typescript/build/bundle.js")
    set(SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/Typescript/build/bundle.js")
    target_sources(TypescriptExample PRIVATE ${SCRIPTS})

    foreach(SCRIPT ${SCRIPTS})
        get_filename_component(SCRIPT_NAME "${SCRIPT}" NAME)
        add_custom_command(
            OUTPUT "${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${SCRIPT}" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Scripts/${SCRIPT_NAME}"
            COMMENT "Copying ${SCRIPT_NAME}"
            MAIN_DEPENDENCY "${SCRIPT}")
    endforeach()
    
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})
else()
    message(FATAL_ERROR "Javascript bundle is required. Please run npm run build on this Typescript.")
endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TypescriptExample)

if(MSVC)
    target_compile_definitions(TypescriptExample PRIVATE TARGET_PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(TypescriptExample PRIVATE TARGET_PLATFORM_OSX)
    find_library(JSCORE_LIBRARY JavaScriptCore)
    target_link_libraries(TypescriptExample PRIVATE ${JSCORE_LIBRARY})
    target_link_libraries(TypescriptExample PRIVATE "-framework Cocoa" "-framework MetalKit" "-framework QuartzCore" "-framework CoreFoundation" "-framework CoreGraphics")
else()
    target_compile_definitions(TypescriptExample PRIVATE TARGET_PLATFORM_LINUX)
endif()