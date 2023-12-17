#### Конфигурируем проект для Google тестов

```console
cmake -D CMAKE_BUILD_TYPE=Debug \
      -D GLOB_EMSCRIPTEN_ROOT_DIR=/Users/<USER_NAME>/Documents/Third-party/emsdk/upstream/emscripten \
      -D GLOB_EMSCRIPTEN_PLATFORM=ON \
      -D GLOB_LIBPNG_ROOT_DIR=/opt/homebrew/Cellar/libpng/1.6.40 \
      -D GLOB_GTEST_ROOT_DIR= \
      -D GLOB_GMOCK_ROOT_DIR= \
      -D GLOB_GTEST_LIB_DIR= \
      -D GLOB_GMOCK_LIB_DIR= \
      -D MODULE_CORE_ENVIRONMENT=node \
      -D MODULE_CORE_COMPILATION=async \
      -D MODULE_LOADER_PNG_ENVIRONMENT=node \
      -D MODULE_LOADER_PNG_COMPILATION=async \
      -D MODULE_LOADER_PNG_SHARED_LIB=ON \
      -D MODULE_LOADER_PNG_MT=ON \
      -D MODULE_LOADER_PNG_ENABLE_TESTS=ON \
      -D MODULE_LOADER_PNG_ENABLE_COVERAGE=OFF \
      -D MODULE_LOADER_PNG_ENABLE_EXAMPLES=OFF \
      ../
```

#### Запускаем сборку

```console
cmake --build ./
```