#---------------------------------------------------------------------------------
# CMake version
#---------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

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
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/clang/create_library.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/setup.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/create_bitcode.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/create_library.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/gen_target_name.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_optimization.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_environment.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_compilation.cmake")
include("${MODULE_LOADER_PNG_CMAKE_DIR}/modules/compiler/emscripten/set_modularize.cmake")

#---------------------------------------------------------------------------------
# Опции сборки
#---------------------------------------------------------------------------------

option(MODULE_LOADER_PNG_SHARED_LIB "Build shared library (.so)" OFF)
option(MODULE_LOADER_PNG_MT "Enable multithreading" OFF)
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

if(GLOB_EMSCRIPTEN_PLATFORM)
  set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
  set(CMAKE_CXX_COMPILER "${GLOB_EMSCRIPTEN_ROOT_DIR}/em++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -Wall -std=c++17 -shared -dynamiclib")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-unicode-whitespace \
      -Wno-unused-local-typedef -Wno-invalid-noreturn -Wno-unused-command-line-argument")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEMSCRIPTEN_PLATFORM")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")

  if(MODULE_LOADER_PNG_SHARED_LIB)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s SIDE_MODULE=1")
  endif()

  if(MODULE_LOADER_PNG_MT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--shared-memory,--no-check-features")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s IMPORTED_MEMORY=1")
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORT_ALL=1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FULL_ES3=1")
endif()

# set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib -L/usr/lib ${CMAKE_EXE_LINKER_FLAGS} -lpng")
# set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
# string(REGEX REPLACE "-lpng" "/opt/homebrew/Cellar/libpng/1.6.40/lib/libpng.a" CMAKE_MAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

# set(PNG_LIBRARY "/opt/homebrew/Cellar/libpng/1.6.40/lib")
# set(PNG_LIBRARY "/opt/homebrew/Cellar/libpng/1.6.40/lib/libpng.a")
set(PNG_LIBRARY "/opt/homebrew/Cellar/libpng/1.6.40/lib/libpng.dylib")
set(PNG_PNG_INCLUDE_DIR "/Users/apriori85/Documents/Third-party/libpng")
find_package(PNG 1.6 REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

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
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/lib/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_loader/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_core/lib/cxx/include")
include_directories("${MODULE_LOADER_PNG_LIST_DIR}/submodules/sway.module_math/lib/cxx/include")

# Получаем пути ко всем хедерам и исходникам библиотеки
file(GLOB_RECURSE MODULE_LOADER_PNG_HEADERS "${MODULE_LOADER_PNG_LIST_DIR}/lib/include/*.*")
file(GLOB_RECURSE MODULE_LOADER_PNG_SOURCES "${MODULE_LOADER_PNG_LIST_DIR}/lib/src/*.*")

if(MODULE_LOADER_PNG_SHARED_LIB)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -fvisibility=hidden")

  # Добавляем в проект цель для сборки динамической библиотеки
  add_library(${MODULE_LOADER_PNG_TARGET} SHARED ${MODULE_LOADER_PNG_SOURCES})
  set_target_properties(${MODULE_LOADER_PNG_TARGET} PROPERTIES OUTPUT_NAME ${MODULE_LOADER_PNG_PROJ_NAME})
  set_target_properties(${MODULE_LOADER_PNG_TARGET} PROPERTIES PREFIX "")
  if(GLOB_EMSCRIPTEN_PLATFORM)
    set_target_properties(${MODULE_LOADER_PNG_TARGET} PROPERTIES SUFFIX ".wasm")
  else()
    set_target_properties(${MODULE_LOADER_PNG_TARGET} PROPERTIES SUFFIX ".dylib.${MODULE_LOADER_PNG_VERSION}")
  endif()

  # target_compile_definitions(${MODULE_LOADER_PNG_TARGET} PRIVATE -DBUILD_DLLAPI_LIBMODULE)
else()
  # Добавляем в проект цель для сборки статической библиотеки
  add_library(${MODULE_LOADER_PNG_TARGET} STATIC ${MODULE_LOADER_PNG_SOURCES})
  set_target_properties(${MODULE_LOADER_PNG_TARGET} PROPERTIES OUTPUT_NAME ${MODULE_LOADER_PNG_PROJ_NAME})
  set_target_properties(${MODULE_LOADER_PNG_TARGET} PROPERTIES PREFIX "")
  set_target_properties(${MODULE_LOADER_PNG_TARGET} PROPERTIES SUFFIX ".a.${MODULE_LOADER_PNG_VERSION}")
endif()

#---------------------------------------------------------------------------------
# Definitions
#---------------------------------------------------------------------------------

set_compile_definitions(${MODULE_LOADER_PNG_TARGET})

#---------------------------------------------------------------------------------
# 
#---------------------------------------------------------------------------------

enable_cxx17(${MODULE_LOADER_PNG_TARGET})
target_link_libraries(${MODULE_LOADER_PNG_TARGET} module_core ${PNG_LIBRARY})

# Добавляем построение тестов
if(MODULE_LOADER_PNG_ENABLE_TESTS)
  if(MODULE_LOADER_PNG_SHARED_LIB)
    message(STATUS "[${MODULE_LOADER_PNG_PROJ_NAME}]: Tests are only allowed with a static lib")
  else()
    message(STATUS "[${MODULE_LOADER_PNG_PROJ_NAME}]: Tests have been enabled")
    add_subdirectory(lib/tests/google)
  endif()
endif()
