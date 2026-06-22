#---------------------------------------------------------------------------------
# CMake version
#---------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.25.0 FATAL_ERROR)

#---------------------------------------------------------------------------------
# Project directories
#---------------------------------------------------------------------------------

set(MODULE_LOADER_PNG_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MODULE_LOADER_PNG_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(MODULE_LOADER_PNG_CMAKE_DIR "${MODULE_LOADER_PNG_ROOT_DIR}/submodules/sway.module_core/cmake_modules")
set(MODULE_LOADER_PNG_LIB_DIR "${MODULE_LOADER_PNG_ROOT_DIR}/lib")
set(MODULE_LOADER_PNG_BIN_DIR "${CMAKE_SOURCE_DIR}/bin")

set(SYSTEM_INC_DIR "/opt/X11/include")
set(SYSTEM_LIB_DIR "/opt/X11/lib")

#---------------------------------------------------------------------------------
# Include CMake modules
#---------------------------------------------------------------------------------

include("${MODULE_LOADER_PNG_CMAKE_DIR}/options.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/set_output_dir.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/add_submodule.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/project_metadata.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/enable_cxx17.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/enable_coverage.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/logger.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/create_object_library.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/create_interface.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/clang/create_library.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/setup.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/create_library.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/gen_target_name.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_optimization.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_environment.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_compilation.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_modularize.cmake")

#---------------------------------------------------------------------------------
# Build options
#---------------------------------------------------------------------------------

set_glob_options()

option(MODULE_LOADER_PNG_LIB_TYPE "Build type library" OFF)
option(MODULE_LOADER_PNG_ENABLE_TESTS "Build tests" OFF)
option(MODULE_LOADER_PNG_ENABLE_EXAMPLES "Build examples" OFF)

#---------------------------------------------------------------------------------
# [Emscripten]: Setup
#---------------------------------------------------------------------------------

if(GLOB_EMSCRIPTEN_PLATFORM)
  emscripten_setup()
endif()

#---------------------------------------------------------------------------------
# Project
#---------------------------------------------------------------------------------

project_metadata("loader_png" 0.2.0)
project(${MODULE_LOADER_PNG_PROJ_NAME} VERSION ${MODULE_LOADER_PNG_VERSION} LANGUAGES CXX)

if(GLOB_EMSCRIPTEN_PLATFORM)
  set(CMAKE_CXX_COMPILER "${EMSCRIPTEN_COMPILER}")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--whole-archive")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-unicode-whitespace \
  #   -Wno-unused-local-typedef -Wno-invalid-noreturn -Wno-unused-command-line-argument")
endif()

#---------------------------------------------------------------------------------
# Submodules
#---------------------------------------------------------------------------------

add_submodule("module_core" submodules/sway.module_core)

#---------------------------------------------------------------------------------
# Executable output path
#---------------------------------------------------------------------------------

set_output_dir(${MODULE_LOADER_PNG_BIN_DIR})

#---------------------------------------------------------------------------------
# Include project headers
#---------------------------------------------------------------------------------

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include_directories("/usr/local/include/c++/v1")
endif()

include_directories("${SYSTEM_INC_DIR}")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/lib/cxx/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_core/lib/cxx/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_math/lib/cpp/inc")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_loader/api/cxx/include")

#---------------------------------------------------------------------------------
# Find the project headers, sources
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# Library
#---------------------------------------------------------------------------------

add_subdirectory("lib/cxx")

#---------------------------------------------------------------------------------
# Tests
#---------------------------------------------------------------------------------

if(MODULE_LOADER_PNG_ENABLE_TESTS)
  message(STATUS "Tests have been enabled")
  add_subdirectory("lib/cxx/tests/google")
endif()

#---------------------------------------------------------------------------------
# Examples
#---------------------------------------------------------------------------------

if(MODULE_LOADER_PNG_ENABLE_EXAMPLES)
  message(STATUS "Examples have been enabled")
  add_subdirectory("examples/01")
endif()
