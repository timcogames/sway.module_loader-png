# sway.module_loader-png

#### Конфигурируем проект для Google тестов

```console
cmake -DCMAKE_BUILD_TYPE=Release -DMODULE_LOADER_PNG_SHARED_LIB=ON ../

cmake -DCMAKE_BUILD_TYPE=Release -DCUSTOM_EMSCRIPTEN_ROOT_DIR=/Users/apriori85/Documents/Third-party/emsdk/upstream/emscripten -DGLOBAL_EMSCRIPTEN_PLATFORM=ON -DMODULE_LOADER_PNG_SHARED_LIB=ON -DMODULE_LOADER_PNG_MT=ON ../
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
