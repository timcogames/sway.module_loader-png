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
option(MODULE_LOADER_PNG_ENABLE_COVERAGE "Enable coverage builds" OFF)
option(MODULE_LOADER_PNG_ENABLE_TESTS "Enable test builds" OFF)
option(CUSTOM_GTEST_ROOT_DIR "GTest root directory" OFF)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

project(${META_LOADER_PNG_PRJNAME} VERSION ${META_LOADER_PNG_VERSION} LANGUAGES CXX)

# Подмодули
custom_add_subdirectory("libmodule_core" submodules/sway.module_core)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

message("[${META_LOADER_PNG_PRJNAME}]: ${META_LOADER_PNG_VERSION}
         compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
       build type | ${CMAKE_BUILD_TYPE}
       shared lib | ${MODULE_CORE_SHARED_LIB}
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

include_directories("${MODULE_LOADER_PNG_DIR}/submodules/sway.module_platform-glx11/include")

# Получаем пути ко всем хедерам и исходникам библиотеки
file(GLOB_RECURSE MODULE_LOADER_PNG_HEADERS "${MODULE_LOADER_PNG_DIR}/lib/include/*.*")
file(GLOB_RECURSE MODULE_LOADER_PNG_SOURCES "${MODULE_LOADER_PNG_DIR}/lib/src/*.*")

if (MODULE_LOADER_PNG_SHARED_LIB)
  set(CMAKE_CXX_COMPILER "g++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Werror -pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-return-type -Wno-int-to-pointer-cast -Wno-variadic-macros -Wno-null-pointer-arithmetic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default -fPIE")

  # Добавляем в проект цель для сборки динамической библиотеки
  add_library(${META_LOADER_PNG_LIBNAME} SHARED ${MODULE_LOADER_PNG_SOURCES})
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES OUTPUT_NAME ${META_LOADER_PNG_PRJNAME})
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES PREFIX "")
  set_target_properties(${META_LOADER_PNG_LIBNAME} PROPERTIES SUFFIX ".dylib.${META_LOADER_PNG_VERSION}")
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
