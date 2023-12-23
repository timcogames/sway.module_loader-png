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

#---------------------------------------------------------------------------------
# Include CMake modules
#---------------------------------------------------------------------------------

include("${MODULE_LOADER_PNG_CMAKE_DIR}/options.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/project_metadata.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/enable_cxx17.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/add_submodule.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/set_compile_definitions.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/set_output_dir.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/create_object.cmake")
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
# Опции сборки
#---------------------------------------------------------------------------------

option(GLOB_LIBPNG_ROOT_DIR "" OFF)

option(MODULE_LOADER_PNG_SHARED_LIB "Build shared library (.so)" OFF)
option(MODULE_LOADER_PNG_ENVIRONMENT "Environment variable" OFF)
option(MODULE_LOADER_PNG_COMPILATION "Compilation variable" OFF)
option(MODULE_LOADER_PNG_MT "Enable multithreading" OFF)
option(MODULE_LOADER_PNG_ALL_EXPORT "" OFF)
option(MODULE_LOADER_PNG_ENABLE_COVERAGE "Enable coverage builds" OFF)
option(MODULE_LOADER_PNG_ENABLE_TESTS "Enable test builds" OFF)

#---------------------------------------------------------------------------------
# [EMSCRIPTEN]: Setup
#---------------------------------------------------------------------------------

if(GLOB_EMSCRIPTEN_PLATFORM)
  emscripten_setup()
endif()

#---------------------------------------------------------------------------------
# Project
#---------------------------------------------------------------------------------

project_metadata("loader_png" 0.1.0)
project(${MODULE_LOADER_PNG_PROJ_NAME} VERSION ${MODULE_LOADER_PNG_VERSION} LANGUAGES CXX)

#---------------------------------------------------------------------------------
# Submodules
#---------------------------------------------------------------------------------

add_submodule("module_core" submodules/sway.module_core)

#---------------------------------------------------------------------------------
# 
#---------------------------------------------------------------------------------

# set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib -L/usr/lib ${CMAKE_EXE_LINKER_FLAGS} -lpng")
# set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
# string(REGEX REPLACE "-lpng" "/opt/homebrew/Cellar/libpng/1.6.40/lib/libpng.a" CMAKE_MAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

set(PNG_LIBRARY "${MODULE_LOADER_PNG_LIBPNG_DIR}/lib")
set(PNG_PNG_INCLUDE_DIR "${MODULE_LOADER_PNG_LIBPNG_DIR}/include")

find_package(PNG 1.6 REQUIRED)
include_directories(${PNG_PNG_INCLUDE_DIR})

message("[${MODULE_LOADER_PNG_PROJ_NAME}]: ${MODULE_LOADER_PNG_VERSION}
         compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
       build type | ${CMAKE_BUILD_TYPE}
       shared lib | ${MODULE_LOADER_PNG_SHARED_LIB}
         cxxflags | ${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}
 build type flags | ${BUILD_TYPE_CXX_FLAGS}
        build dir | ${CMAKE_BINARY_DIR}
   install prefix | ${CMAKE_INSTALL_PREFIX}

        gtest dir | ${GLOB_GTEST_ROOT_DIR}

         coverage | ${MODULE_LOADER_PNG_ENABLE_COVERAGE}
            tests | ${MODULE_LOADER_PNG_ENABLE_TESTS}
")

#---------------------------------------------------------------------------------
# Определяем каталог вывода по умолчанию
#---------------------------------------------------------------------------------

set_output_dir(${MODULE_LOADER_PNG_BIN_DIR})

set(GLUT_INCLUDE_DIRS "/opt/X11/include")
set(GLUT_LIBRARY_DIRS "/opt/X11/lib")

# Добавляем пути для поиска хедеров
include_directories("${GLUT_INCLUDE_DIRS}")
# include_directories("/Library/Developer/CommandLineTools/usr/include/c++/v1")
include_directories("/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/lib/cxx/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_loader/api/cxx/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_core/lib/cxx/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_math/lib/cxx/include")

# Получаем пути ко всем хедерам и исходникам библиотеки
file(GLOB_RECURSE MODULE_LOADER_PNG_HEADERS "${MODULE_LOADER_PNG_LIST_DIR}/lib/cxx/include/*.*")
file(GLOB_RECURSE MODULE_LOADER_PNG_SOURCES "${MODULE_LOADER_PNG_LIST_DIR}/lib/cxx/src/*.*")

#---------------------------------------------------------------------------------
# [EMSCRIPTEN] Bitcode
#---------------------------------------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default -fPIE")

if(GLOB_EMSCRIPTEN_PLATFORM)
  create_object(${MODULE_LOADER_PNG_TARGET} "${MODULE_LOADER_PNG_ROOT_DIR}/lib/cxx/src/*.*" TRUE)
else()
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -dynamiclib")
  # create_library(${MODULE_LOADER_PNG_TARGET} "${MODULE_LOADER_PNG_ROOT_DIR}/lib/cxx/src/*.*")
  # target_link_libraries(${MODULE_LOADER_PNG_TARGET} X11 GL module_core)
endif()

if(GLOB_EMSCRIPTEN_PLATFORM)
  create_emscripten_library(${MODULE_LOADER_PNG_TARGET} ${MODULE_LOADER_PNG_ENVIRONMENT} ${MODULE_LOADER_PNG_COMPILATION} OUTPUT_TARGET_NAME_LIST)

  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --profiling-funcs")

  foreach(target IN LISTS OUTPUT_TARGET_NAME_LIST)
    target_link_options(${target} PUBLIC 
      "SHELL:-s MODULARIZE=1"
      "SHELL:-s FULL_ES3=1"
      "SHELL:-s SUPPORT_LONGJMP=0"
      "SHELL:-s EXPORT_NAME=\"create_loader_png_module\""
      "SHELL:-s EXPORTED_RUNTIME_METHODS=@${MODULE_LOADER_PNG_LIB_DIR}/runtime-exports.json")

    # target_link_options(${target} PUBLIC 
    #   "SHELL:-s STRICT=1")
    # OR
    target_link_options(${target} PUBLIC "SHELL:-s BINARYEN_ASYNC_COMPILATION=1")

    if(MODULE_LOADER_PNG_ALL_EXPORT)
      target_link_options(${target} PUBLIC "SHELL:-s LINKABLE=1") # Полный экспорт
    else()
      target_link_options(${target} PUBLIC "SHELL:-s EXPORTED_FUNCTIONS=@${MODULE_LOADER_PNG_LIB_DIR}/exports.json") # Частичный экспорт
    endif()

    if(MODULE_LOADER_PNG_SHARED_LIB)
      target_link_options(${target} PUBLIC 
        "SHELL:-shared"
        "SHELL:-dynamiclib")

      if(MODULE_LOADER_PNG_ALL_EXPORT)
        target_link_options(${target} PUBLIC "SHELL:-s SIDE_MODULE=2")
      else()
        target_link_options(${target} PUBLIC "SHELL:-s SIDE_MODULE=1")
      endif()
    endif()

    target_link_options(${target} PUBLIC "SHELL:-s USE_LIBPNG=1")

    if(MODULE_LOADER_PNG_MT)
      target_link_options(${target} PUBLIC 
        "SHELL:-Wl,--shared-memory,--no-check-features"
        "SHELL:-s USE_PTHREADS=1"
        "SHELL:-s PTHREAD_POOL_SIZE=8"
        "SHELL:-s IMPORTED_MEMORY=1")
    endif()

    enable_cxx17(${target})

    target_link_directories(${target} PRIVATE ${PNG_LIBRARY})
    # target_link_libraries(${target} module_core -L${GLOB_LIBPNG_ROOT_DIR}/lib -lpng)

    printf_link_options(${target})
  endforeach()
else()
  # set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/gcc/13.2.0/bin/g++-13")
  set(CMAKE_CXX_COMPILER "/Library/Developer/CommandLineTools/usr/bin/g++")

  create_library(${MODULE_LOADER_PNG_TARGET} "${MODULE_LOADER_PNG_ROOT_DIR}/lib/cxx/src/*.*")
  target_link_libraries(${MODULE_LOADER_PNG_TARGET} module_core -L${GLOB_LIBPNG_ROOT_DIR}/lib -lpng16)
  enable_cxx17(${MODULE_LOADER_PNG_TARGET})
endif()

# Добавляем построение тестов
if(MODULE_LOADER_PNG_ENABLE_TESTS)
  if(MODULE_LOADER_PNG_SHARED_LIB)
    message(STATUS "[${MODULE_LOADER_PNG_PROJ_NAME}]: Tests are only allowed with a static lib")
  else()
    message(STATUS "[${MODULE_LOADER_PNG_PROJ_NAME}]: Tests have been enabled")
    add_subdirectory(lib/cxx/tests/google)
  endif()
endif()
