#### Конфигурируем проект для Google тестов

```console
cmake -D GLOB_GTEST_ROOT_DIR=/Users/<USER_NAME>/Documents/Third-party/googletest \
      -D MODULE_LOADER_PNG_LIBPNG_DIR=/opt/homebrew/Cellar/libpng/1.6.40 \
      -D MODULE_LOADER_PNG_ENABLE_TESTS=ON \
      ../
```

#### Запускаем сборку

```console
cmake --build ./
```
