#### Конфигурируем проект для Google тестов

```console
ctest --preset default-test-macos

# deprecated
cmake -D CMAKE_BUILD_TYPE=Debug \
      -D GLOB_EMSCRIPTEN_ROOT_DIR= \
      -D GLOB_EMSCRIPTEN_PLATFORM=OFF \
      -D GLOB_LIBPNG_ROOT_DIR=/opt/homebrew/Cellar/libpng/1.6.40 \
      -D GLOB_GTEST_ROOT_DIR=/Users/apriori85/Documents/Third-party/googletest/googletest \
      -D GLOB_GMOCK_ROOT_DIR=/Users/apriori85/Documents/Third-party/googletest/googlemock \
      -D GLOB_GTEST_LIB_DIR=/Users/apriori85/Documents/Third-party/googletest/build/lib \
      -D GLOB_GMOCK_LIB_DIR=/Users/apriori85/Documents/Third-party/googletest/build/lib \
      -D MODULE_CORE_ENVIRONMENT=node \
      -D MODULE_CORE_COMPILATION=async \
      -D MODULE_LOADER_PNG_ENVIRONMENT=node \
      -D MODULE_LOADER_PNG_COMPILATION=async \
      -D MODULE_LOADER_PNG_SHARED_LIB=OFF \
      -D MODULE_LOADER_PNG_MT=ON \
      -D MODULE_LOADER_PNG_ENABLE_TESTS=ON \
      -D MODULE_LOADER_PNG_ENABLE_COVERAGE=OFF \
      -D MODULE_LOADER_PNG_ENABLE_EXAMPLES=OFF \
      ../
```

#### Запускаем сборку

```console
cmake --build build --preset default-macos-static-mt-debug

# deprecated
cmake --build ./
```
