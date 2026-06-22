"use strict";

export const useBridgeEmscripten = <TBridgeModule>(moduleName: string): Promise<{ create_loader_png_module: TBridgeModule }> => {
  return new Promise(async (resolve) => {
    // Загружаем JavaScript обертку Emscripten вместо прямого WASM
    const modulePath = moduleName.replace('.wasm', '.js');
    
    // Динамически импортируем Emscripten модуль
    const emscriptenModule = await import(modulePath);
    
    // Emscripten модуль обычно экспортирует функцию, которая возвращает Promise
    const create_loader_png_module = await emscriptenModule.default();
    resolve({ create_loader_png_module });
  });
};
