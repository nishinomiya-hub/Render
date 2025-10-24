cmake_minimum_required(VERSION 3.10)
project(Render)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME Render)
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

# enable SIMD
add_definitions("-DSOFTGL_SIMD_OPT")

# debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions("-DDEBUG")
endif ()


# 包含 FetchContent 模块
include(FetchContent)

# 下载并配置 Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
  GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

find_program(CCACHE "ccache")
if (CCACHE)
    message(STATUS "find ccache")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif (CCACHE)

include_directories(
        "${THIRD_PARTY_DIR}/glfw/include"
        "${THIRD_PARTY_DIR}/glad/include"
        "${THIRD_PARTY_DIR}/glm"
        "${THIRD_PARTY_DIR}/assimp/include"
        "${THIRD_PARTY_DIR}/assimp/contrib"
        "${THIRD_PARTY_DIR}/imgui"
        "${THIRD_PARTY_DIR}/json11"
        "${THIRD_PARTY_DIR}/md5"
        "${THIRD_PARTY_DIR}/stb/include"
        # "${THIRD_PARTY_DIR}/vulkan/include"
        # "${THIRD_PARTY_DIR}/glslang"
        # "${THIRD_PARTY_DIR}/vma"
        "${THIRD_PARTY_DIR}/renderdoc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# # assimp
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_ZLIB ON)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT FALSE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)
set(ASSIMP_BUILD_GLTF_IMPORTER TRUE)

add_subdirectory(${THIRD_PARTY_DIR}/assimp)


# # glslang
# set(SKIP_GLSLANG_INSTALL ON)
# set(ENABLE_SPVREMAPPER OFF)
# set(ENABLE_GLSLANG_BINARIES OFF)
# set(ENABLE_CTEST OFF)
# set(ENABLE_PCH OFF)
# set(BUILD_TESTING OFF)
# set(USE_CCACHE ON)
# add_subdirectory("${THIRD_PARTY_DIR}/glslang")

# main src
file(GLOB SOFTGL_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Base/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Soft/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/OpenGL/*.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Vulkan/*.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/Render/Vulkan/SPIRV/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/*.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Shader/GLSL/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Shader/Software/*.cpp
        )

#imgui src
file(GLOB IMGUI_SRC
        ${THIRD_PARTY_DIR}/imgui/imgui/*.cpp
        )


add_library(RenderLib STATIC 
        "${SOFTGL_SRC}"
        "${IMGUI_SRC}"
        "${THIRD_PARTY_DIR}/glad/src/glad.c"
        "${THIRD_PARTY_DIR}/json11/json11.cpp"
        "${THIRD_PARTY_DIR}/md5/md5.c"
)

add_executable(${TARGET_NAME}
        "${SOFTGL_SRC}"
        "${IMGUI_SRC}"
        "${THIRD_PARTY_DIR}/glad/src/glad.c"
        "${THIRD_PARTY_DIR}/json11/json11.cpp"
        "${THIRD_PARTY_DIR}/md5/md5.c"
        )

set(LINK_LIBS
        assimp
        #glslang
        # glslang-default-resource-limits
        # SPIRV
        )

if (WIN32)
    # set(LINK_LIBS ${LINK_LIBS}
    #         "${THIRD_PARTY_DIR}/vulkan/lib-win-x64/vulkan-1.lib"
    #         )
    if (MSVC)
        set(LINK_LIBS ${LINK_LIBS}
                "${THIRD_PARTY_DIR}/glfw/lib-vc2022/glfw3.lib"
                "${THIRD_PARTY_DIR}/glfw/lib-vc2022/glfw3dll.lib"
                )
    else ()
        set(LINK_LIBS ${LINK_LIBS}
                "${THIRD_PARTY_DIR}/glfw/lib-mingw-w64/libglfw3.a"
                "${THIRD_PARTY_DIR}/glfw/lib-mingw-w64/libglfw3dll.a"
                )
    endif ()
endif ()

# if (APPLE)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")
#     add_compile_definitions(GL_SILENCE_DEPRECATION)
#     set(LINK_LIBS ${LINK_LIBS}
#             "${THIRD_PARTY_DIR}/vulkan/lib-macos-universal/libvulkan.1.dylib"
#             "${THIRD_PARTY_DIR}/glfw/lib-macos-universal/libglfw3.a"
#             )
# endif ()

if (UNIX AND NOT APPLE)
    set(OpenGL_GL_PREFERENCE "LEGACY")
    find_package(OpenGL REQUIRED)
    set(LINK_LIBS ${LINK_LIBS}
            pthread
            glfw
            OpenGL::GL
            #"${THIRD_PARTY_DIR}/vulkan/lib-linux-x64/libvulkan.so.1"
            ${CMAKE_DL_LIBS}
            )
endif ()

if (MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE $<$<BOOL:${MSVC}>:/arch:AVX2 /std:c++20>)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -O3")
endif ()

target_link_libraries(${TARGET_NAME} ${LINK_LIBS})

target_include_directories(RenderLib PUBLIC
        "${THIRD_PARTY_DIR}/glfw/include"
        "${THIRD_PARTY_DIR}/glad/include"
        "${THIRD_PARTY_DIR}/glm"
        "${THIRD_PARTY_DIR}/assimp/include"
        "${THIRD_PARTY_DIR}/assimp/contrib"
        "${THIRD_PARTY_DIR}/imgui"
        "${THIRD_PARTY_DIR}/json11"
        "${THIRD_PARTY_DIR}/md5"
        "${THIRD_PARTY_DIR}/stb/include"
        # "${THIRD_PARTY_DIR}/vulkan/include"
        # "${THIRD_PARTY_DIR}/glslang"
        # "${THIRD_PARTY_DIR}/vma"
        "${THIRD_PARTY_DIR}/renderdoc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(RenderLib PRIVATE ${LINK_LIBS})

# 收集测试源文件
file(GLOB TEST_SOURCES
    tests/*.cpp
    # 添加其他测试文件
)

include(GoogleTest)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
else()
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)
endif()

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets ${EXECUTABLE_OUTPUT_PATH}/assets
        
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Shader/GLSL ${EXECUTABLE_OUTPUT_PATH}/shaders/GLSL
        
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/cache ${EXECUTABLE_OUTPUT_PATH}/cache
        
    COMMAND ${CMAKE_COMMAND} -E copy
        ${THIRD_PARTY_DIR}/renderdoc/renderdoc.dll ${EXECUTABLE_OUTPUT_PATH}/renderdoc.dll
)

# 修改测试目标输出路径
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE gtest_main RenderLib)
    
    # 设置测试输出目录
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/tests
    )
    gtest_discover_tests(${test_name})
endforeach()

# 修改全局测试目标输出路径
if(TEST_SOURCES)
    add_executable(run_all_tests ${TEST_SOURCES})
    target_link_libraries(run_all_tests PRIVATE gtest_main RenderLib)
    set_target_properties(run_all_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/tests
    )
    gtest_discover_tests(run_all_tests)
endif()
