cmake_minimum_required(VERSION 3.29)

option(ENABLE_CPP23_MODULE "Enable C++ 23 module support for Vulkan" ON)
if(ENABLE_CPP23_MODULE)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON CACHE BOOL "Enable C++ module import scanning" FORCE)
endif()

project(
        Dodo
        VERSION 0.0.1
        DESCRIPTION "A simple C++ project template"
        HOMEPAGE_URL "https://github.com/NicolasReboule/Dodo"
        LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

option(BUILD_COVERAGE "Build the coverage" OFF)
option(BUILD_TESTS "Build the tests" OFF)
option(USE_CLANG_TIDY "Use clang-tidy" OFF) # Used by the CI
option(USE_CLANG_TIDY_WITH_ERRORS "Use clang tidy but all warnings are errors" OFF) # Used by the CI
option(USE_CLANG_TIDY_WITH_FIX "Use clang-tidy with fix" OFF) # Used by the CI

# Headers
add_library(${PROJECT_NAME}_Headers INTERFACE)
target_include_directories(${PROJECT_NAME}_Headers INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Coverage
if(BUILD_COVERAGE)
    target_compile_options(${PROJECT_NAME}_Headers INTERFACE --coverage)
    target_link_options(${PROJECT_NAME}_Headers INTERFACE --coverage)
endif()

# Sources
add_library(${PROJECT_NAME}_Sources STATIC
        src/dodo/core/app.cpp
        src/dodo/core/DodoContext.cpp
        src/dodo/core/VulkanContext.cpp
        src/dodo/core/GLFWContext.cpp
)
target_link_libraries(${PROJECT_NAME}_Sources PUBLIC ${PROJECT_NAME}_Headers)

# Vulkan
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
find_package (Vulkan REQUIRED)

if(ENABLE_CPP23_MODULE)
    # set up Vulkan C++ module (enabled when ENABLE_CPP23_MODULE=ON)
    add_library(VulkanCppModule)
    add_library(Vulkan::cppm ALIAS VulkanCppModule)

    target_compile_definitions(VulkanCppModule PUBLIC
            VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
            VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
    )

    target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")

    target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)

    set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 23)

    # Add MSVC-specific compiler options for proper C++ module support
    if(MSVC)
        target_compile_options(VulkanCppModule PRIVATE
                /std:c++latest      # Use latest C++ standard for better module support
                /permissive-        # Standards conformance mode
                /Zc:__cplusplus     # Enable correct __cplusplus macro
                /EHsc               # Enable C++ exception handling
                /Zc:preprocessor    # Use conforming preprocessor
                /translateInclude   # Automatically translate #include to import for standard library
        )
    endif()

    target_sources(VulkanCppModule
            PUBLIC
            FILE_SET cxx_modules TYPE CXX_MODULES
            BASE_DIRS "${Vulkan_INCLUDE_DIR}"
            FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
    )
    # Add the vulkan.cppm file directly as a source file
    target_sources(VulkanCppModule
            PRIVATE
            "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
    )
else()
    # Create a dummy interface library when C++ 23 module is disabled
    add_library(VulkanCppModule INTERFACE)
    add_library(Vulkan::cppm ALIAS VulkanCppModule)
    target_link_libraries(VulkanCppModule INTERFACE Vulkan::Vulkan)
    target_compile_definitions(VulkanCppModule
            INTERFACE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
    )
endif()

find_package (glm REQUIRED)

target_link_directories(${PROJECT_NAME}_Sources PUBLIC lib)

if(MSVC)
    target_link_libraries(${PROJECT_NAME}_Sources PUBLIC Vulkan::cppm glm::glm)
else()
    target_link_libraries(${PROJECT_NAME}_Sources PUBLIC Vulkan::cppm glm::glm libglfw3.a)
endif()

# Main executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_Sources)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3.lib)

# Compiler Warnings
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Tests
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# CLang-tidy
if (USE_CLANG_TIDY OR USE_CLANG_TIDY_WITH_ERRORS OR USE_CLANG_TIDY_WITH_FIX)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy-19")
    if (NOT CLANG_TIDY_EXE)
        message(WARNING "clang-tidy not found.")
    else ()
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        if (USE_CLANG_TIDY_WITH_FIX)
            set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--fix" "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
        elseif (USE_CLANG_TIDY_WITH_ERRORS)
            set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--warnings-as-errors=*" "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
        else ()
            set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
        endif ()
        set_target_properties(${BINARY_NAME} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    endif ()
endif ()
