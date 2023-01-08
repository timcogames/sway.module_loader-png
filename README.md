# sway.module_loader-png

#### Конфигурируем проект для Google тестов

```console
cmake -DCMAKE_BUILD_TYPE=Release -DMODULE_LOADER_PNG_SHARED_LIB=ON ../
```

#### Запускаем сборку

```console
cmake --build ./
```

#### Таблица символов

```console
# macos
nm -g ./../bin/module_gapi_gl.dylib.*
```
