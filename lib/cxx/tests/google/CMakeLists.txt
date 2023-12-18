# Мета-информация о проекте
set(META_LOADER_PNG_PRJNAME_TESTS "module_loader_png_tests")
set(META_LOADER_PNG_EXENAME_TESTS "${META_LOADER_PNG_PRJNAME_TESTS}")

project(${META_LOADER_PNG_PRJNAME_TESTS} CXX)

enable_testing()

set(GTEST_ROOT "${GLOB_GTEST_ROOT_DIR}")
set(GTEST_INC_DIR "${GTEST_ROOT}/include")
set(GTEST_SRC_DIR "${GTEST_ROOT}/src")

include_directories("${GTEST_INC_DIR}")

# Получаем пути ко всем исходникам с тестами
file(GLOB MODULE_LOADER_PNG_SOURCE_TESTS "${PROJECT_SOURCE_DIR}/*.spec.cpp")

add_executable(${META_LOADER_PNG_EXENAME_TESTS} ${MODULE_LOADER_PNG_SOURCE_TESTS} "${GTEST_SRC_DIR}/gtest_main.cc")
target_link_libraries(${META_LOADER_PNG_EXENAME_TESTS} module_loader_png module_core pthread ${GLOB_GTEST_LIB_DIR}/libgtest.a)

enable_cxx17(${META_LOADER_PNG_EXENAME_TESTS})

add_test(NAME ${META_LOADER_PNG_EXENAME_TESTS} COMMAND ${META_LOADER_PNG_EXENAME_TESTS})
