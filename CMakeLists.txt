# Проверяем версию CMake
cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/submodules/sway.module_core/cmakehelpers/custom_enable_cxx17.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/submodules/sway.module_core/cmakehelpers/custom_add_subdirectory.cmake")

# Мета-информация о проекте
set(META_LOADER_PNG_PRJNAME "module_loader_png")
set(META_LOADER_PNG_LIBNAME "lib${META_LOADER_PNG_PRJNAME}")
set(META_LOADER_PNG_VERSION_MAJOR 0)
set(META_LOADER_PNG_VERSION_MINOR 1)
set(META_LOADER_PNG_VERSION_PATCH 0)
set(META_LOADER_PNG_VERSION ${META_LOADER_PNG_VERSION_MAJOR}.${META_LOADER_PNG_VERSION_MINOR}.${META_LOADER_PNG_VERSION_PATCH})

# Опции сборки
option(MODULE_LOADER_PNG_SHARED_LIB "Build shared library (.so)" OFF)
option(MODULE_LOADER_PNG_MT "Enable multithreading" OFF)
option(MODULE_LOADER_PNG_ENABLE_COVERAGE "Enable coverage builds" OFF)
option(MODULE_LOADER_PNG_ENABLE_TESTS "Enable test builds" OFF)
option(GLOBAL_EMSCRIPTEN_PLATFORM "Using emscripten compiler" OFF)
option(GLOBAL_EMSCRIPTEN_PLATFORM_USE_BINDING "Using emscripten binding" OFF)
option(CUSTOM_EMSCRIPTEN_ROOT_DIR "Emscripten root directory" OFF)
option(CUSTOM_GTEST_ROOT_DIR "GTest root directory" OFF)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE AND GLOBAL_EMSCRIPTEN_PLATFORM)
  set(CMAKE_TOOLCHAIN_FILE "${CUSTOM_EMSCRIPTEN_ROOT_DIR}/cmake/Modules/Platform/Emscripten.cmake")
  include(${CMAKE_TOOLCHAIN_FILE})
endif ()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

project(${META_LOADER_PNG_PRJNAME} VERSION ${META_LOADER_PNG_VERSION} LANGUAGES CXX)

# Подмодули
custom_add_subdirectory("libmodule_core" submodules/sway.module_core)

if (GLOBAL_EMSCRIPTEN_PLATFORM)
  set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
  set(CMAKE_CXX_COMPILER "${CUSTOM_EMSCRIPTEN_ROOT_DIR}/em++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -Wall -std=c++17 -shared -dynamiclib")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-unicode-whitespace \
      -Wno-unused-local-typedef -Wno-invalid-noreturn -Wno-unused-command-line-argument")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEMSCRIPTEN_PLATFORM")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sWASM=1")
  if (MODULE_LOADER_PNG_SHARED_LIB)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sSIDE_MODULE=1")
  endif ()
  if (MODULE_LOADER_PNG_MT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--shared-memory,--no-check-features")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_PTHREADS=1 -sPTHREAD_POOL_SIZE=8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sIMPORTED_MEMORY=1")
  endif ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sEXPORT_ALL=1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sFULL_ES3=1")
endif ()

# set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib -L/usr/lib ${CMAKE_EXE_LINKER_FLAGS} -lpng")
# set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
# string(REGEX REPLACE "-lpng" "/opt/homebrew/Cellar/libpng/1.6.40/lib/libpng.a" CMAKE_MAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

set(PNG_LIBRARY "/opt/homebrew/Cellar/libpng/1.6.40/lib")
# set(PNG_LIBRARY "/opt/homebrew/Cellar/libpng/1.6.40/lib/libpng.a")
# set(PNG_LIBRARY "/opt/homebrew/Cellar/libpng/1.6.40/lib/libpng.dylib")
set(PNG_PNG_INCLUDE_DIR "/Users/apriori85/Documents/Third-party/libpng")
find_package(PNG 1.6 REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

message("[${META_LOADER_PNG_PRJNAME}]: ${META_LOADER_PNG_VERSION}
         compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
       build type | ${CMAKE_BUILD_TYPE}
       shared lib | ${MODULE_LOADER_PNG_SHARED_LIB}
         cxxflags | ${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}
 build type flags | ${BUILD_TYPE_CXX_FLAGS}
        build dir | ${CMAKE_BINARY_DIR}
   install prefix | ${CMAKE_INSTALL_PREFIX}

        gtest dir | ${CUSTOM_GTEST_ROOT_DIR}

         coverage | ${MODULE_LOADER_PNG_ENABLE_COVERAGE}
            tests | ${MODULE_LOADER_PNG_ENABLE_TESTS}
")

# Определяем каталог вывода по умолчанию
if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/dbg")
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/dbg")
else ()
  set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
endif ()

set(GLUT_INCLUDE_DIRS "/opt/X11/include")
set(GLUT_LIBRARY_DIRS "/opt/X11/lib")
set(MODULE_LOADER_PNG_DIR ${CMAKE_CURRENT_LIST_DIR})

# Добавляем пути для поиска хедеров
include_directories("${GLUT_INCLUDE_DIRS}")
include_directories("${MODULE_LOADER_PNG_DIR}/lib/include")
include_directories("${MODULE_LOADER_PNG_DIR}/submodules/sway.module_loader/include")
include_directories("${MODULE_LOADER_PNG_DIR}/submodules/sway.module_core/lib/include")
include_directories("${MODULE_LOADER_PNG_DIR}/submodules/sway.module_math/lib/include")

# Получаем пути ко всем хедерам и исходникам библиотеки
file(GLOB_RECURSE MODULE_LOADER_PNG_HEADERS "${MODULE_LOADER_PNG_DIR}/lib/include/*.*")
file(GLOB_RECURSE MODULE_LOADER_PNG_SOURCES "${MODULE_LOADER_PNG_DIR}/lib/src/*.*")

if (MODULE_LOADER_PNG_SHARED_LIB)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -fvisibility=hidden")

  # Добавляем в проект цель для сборки динамической библиотеки
  add_library(${META_LOADER_PNG_LIBNAME} SHARED ${MODULE_LOADER_PNG_SOURCES})
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES OUTPUT_NAME ${META_LOADER_PNG_PRJNAME})
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES PREFIX "")
  if (GLOBAL_EMSCRIPTEN_PLATFORM)
    set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES SUFFIX ".wasm")
  else ()
    set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES SUFFIX ".dylib.${META_LOADER_PNG_VERSION}")
  endif ()
  target_compile_definitions(${META_LOADER_PNG_LIBNAME} PRIVATE -DBUILD_DLLAPI_LIBMODULE)
else ()
  # Добавляем в проект цель для сборки статической библиотеки
  add_library(${META_LOADER_PNG_LIBNAME} STATIC ${MODULE_LOADER_PNG_SOURCES})
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES OUTPUT_NAME ${META_LOADER_PNG_PRJNAME})
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES PREFIX "")
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES SUFFIX ".a.${META_LOADER_PNG_VERSION}")
endif ()

custom_enable_cxx17(${META_LOADER_PNG_LIBNAME})
target_link_libraries(${META_LOADER_PNG_LIBNAME} libmodule_core ${PNG_LIBRARY})

# Добавляем построение тестов
if (MODULE_LOADER_PNG_ENABLE_TESTS)
  if (MODULE_LOADER_PNG_SHARED_LIB)
    message(STATUS "[${META_LOADER_PNG_PRJNAME}]: Tests are only allowed with a static lib")
  else ()
    message(STATUS "[${META_LOADER_PNG_PRJNAME}]: Tests have been enabled")
    add_subdirectory(lib/tests/google)
  endif ()
endif ()
