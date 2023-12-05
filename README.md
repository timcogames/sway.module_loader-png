# sway.module_loader-png

#### Конфигурируем проект для Google тестов

```console
cmake -D CMAKE_BUILD_TYPE=Release \
      -D MODULE_LOADER_PNG_SHARED_LIB=ON ../

cmake -D CMAKE_BUILD_TYPE=Release \
      -D GLOB_EMSCRIPTEN_ROOT_DIR=/Users/<USER_NAME>/Documents/Third-party/emsdk/upstream/emscripten \
      -D GLOB_EMSCRIPTEN_PLATFORM=ON \
      -D MODULE_LOADER_PNG_SHARED_LIB=ON \
      -D MODULE_LOADER_PNG_MT=ON ../
```

#### Запускаем сборку

```console
cmake --build ./
```

#### Таблица символов

```console
# macos
nm -g ./../bin/module_loader_png.dylib.*
```
