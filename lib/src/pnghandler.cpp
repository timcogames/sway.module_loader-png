#include <sway/loader/png/pnghandler.hpp>

#include <iostream>

#define PNG_SIGNATURE_SIZE 8  // Длина сигнатуры.

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

PNGHandler::PNGHandler() {}

auto PNGHandler::loadFromStream(std::ifstream &source) -> ImageDescriptor {
  png_byte signature[PNG_SIGNATURE_SIZE] = {0};  // Массив под сигнатуру.
  int isSignatureValid = 0;  // Результат проверки сигнатуры.

  // Считываем сигнатуру (первые PNG_SIGNATURE_SIZE байт).
  source.read((s8_t *)signature, PNG_SIGNATURE_SIZE);

  // Проверяем на соответствие считанной нами сигнатуры с сигнатурой PNG-формата.
  isSignatureValid = png_sig_cmp(signature, 0, PNG_SIGNATURE_SIZE);
  if (isSignatureValid != 0) {
    source.close();
  }

  png_structp png;
  // Выделяем память и инициализируем структуру для работы с файлом.
  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGHandler::error, PNGHandler::warning);
  if (!png) {
    source.close();
  }

  png_infop info, endInfo;

  // Выделяем память и инициализируем структуру с информацией о файле.
  info = png_create_info_struct(png);
  if (!info) {
    png_destroy_read_struct(&png, (png_infopp) nullptr, (png_infopp) nullptr);
    source.close();
  }

  // Выделяем память и инициализируем структуру с информацией о файле.
  endInfo = png_create_info_struct(png);
  if (!endInfo) {
    png_destroy_read_struct(&png, &info, (png_infopp) nullptr);
    source.close();
  }

  // Устанавливаем механизм обработки ошибок.
  if (setjmp(png_jmpbuf(png))) {
    png_destroy_read_struct(&png, &info, &endInfo);
    source.close();
  }

  png_set_read_fn(png, (png_voidp)&source, PNGHandler::readData);  // Устанавливаем собственную функцию чтения данных.
  png_set_sig_bytes(png,
      PNG_SIGNATURE_SIZE);  // Сообщаем Libpng, что мы уже прочитали PNG_SIGNATURE_SIZE байт, когда проверяли сигнатуру.
  png_read_info(png, info);  // Читаем информацию о данных изображения.

  u32_t width = png_get_image_width(png, info);  // Ширина изображения в пикселях.
  u32_t height = png_get_image_height(png, info);  // Высота изображения в пикселях.

  int bitDepth = png_get_bit_depth(png, info);  // Глубина цвета.
  int colorType = png_get_color_type(png, info);
  // int interlaceMethod = png_get_interlace_type(png, info);
  // int compressionMethod = png_get_compression_type(png, info);
  // int filterMethod = png_get_filter_type(png, info);
  int channels = png_get_channels(png, info);

  switch (colorType) {
    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(png);
      png_set_filler(png, 0xff, PNG_FILLER_AFTER);
      break;
    case PNG_COLOR_TYPE_GRAY:
      png_set_filler(png, 0xff, PNG_FILLER_AFTER);
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      if (bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
      if (bitDepth == 16)
        png_set_strip_16(png);
      png_set_gray_to_rgb(png);
      break;
    case PNG_COLOR_TYPE_RGB:
      if (bitDepth < 8)
        png_set_packing(png);
      if (bitDepth == 16)
        png_set_strip_16(png);
      png_set_filler(png, 0xff, PNG_FILLER_AFTER);
      break;
    default:
      break;
  }

  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
    channels += 1;
  }

  png_read_update_info(png, info);  // Обновляем информацию структуры png.

  // Получаем кол-во байтов необходимых для вмещения преобразованного ряда.
  int rowBytes = png_get_rowbytes(png, info);

  // Выделяем памяти под данные изображения.
  auto *tempData = (u8_t *)malloc(rowBytes * height);
  if (!tempData) {
    png_destroy_read_struct(&png, &info, &endInfo);
    free(tempData);
    source.close();
  }

  // Выделяем память под указатели на каждую строку.
  png_bytep *rowPointers = new png_bytep[height];
  if (!rowPointers) {
    png_destroy_read_struct(&png, &info, &endInfo);
    free(tempData);
    SAFE_DELETE_ARRAY(rowPointers);
    source.close();
  }

  for (u32_t i = 0; i < height; ++i) {
    rowPointers[i] = &tempData[i * rowBytes];
  }

  png_read_image(png, rowPointers);  // Читаем изображение.
  png_read_end(png, endInfo);

  ImageDescriptor descriptor = {};
  descriptor.pixels = tempData;
  descriptor.width = width;
  descriptor.height = height;
  descriptor.bpp = channels * bitDepth;
  // descriptor.type = (bitDepth == 16) ? UInt16 : UByte;
  // descriptor.internalFormat = RGBA8;
  // descriptor.format = RGBA;

  png_destroy_read_struct(&png, &info, &endInfo);  // Освобождаем память выделенную для библиотеки Libpng.
  SAFE_DELETE_ARRAY(rowPointers);  // Освобождаем память от указателей на строки.
  source.close();

  return descriptor;
}

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)