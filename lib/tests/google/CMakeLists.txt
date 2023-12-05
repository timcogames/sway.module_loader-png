# Мета-информация о проекте
set(META_LOADER_PNG_PRJNAME_TESTS "module_loader_png_tests")
set(META_LOADER_PNG_EXENAME_TESTS "${META_LOADER_PNG_PRJNAME_TESTS}")

project(${META_LOADER_PNG_PRJNAME_TESTS} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(GTest_ROOT "${GLOB_GTEST_ROOT_DIR}")
set(GTEST_INCLUDE_DIR "${GTest_ROOT}/googletest/include")
set(GTEST_LIBRARY_DIR "${GTest_ROOT}/build/lib")

find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})

# Получаем пути ко всем исходникам с тестами
file(GLOB MODULE_LOADER_PNG_SOURCE_TESTS "${PROJECT_SOURCE_DIR}/*.spec.cpp")

add_executable(${META_LOADER_PNG_EXENAME_TESTS} ${MODULE_LOADER_PNG_SOURCE_TESTS} "${GTest_ROOT}/googletest/src/gtest_main.cc")
target_link_libraries(${META_LOADER_PNG_EXENAME_TESTS} module_loader_png module_core ${GTEST_LIBRARY_DIR}/libgtest.a ${CMAKE_DL_LIBS})

add_custom_command(TARGET ${META_LOADER_PNG_EXENAME_TESTS} POST_BUILD COMMAND ${META_LOADER_PNG_EXENAME_TESTS})
