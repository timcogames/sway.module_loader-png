#include <sway/loader/png/pngplugin.hpp>

#include <iostream>

#define PNG_SIGNATURE_SIZE 8  // Длина сигнатуры.

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

DECLARE_LOADER_PLUGIN(PNGPlugin, "png", core::Version(1))

void PNGPlugin::readSignature_(std::ifstream &source) {
  // png_byte signature[PNG_SIGNATURE_SIZE] = {0};  // Массив под сигнатуру.
  std::array<png_byte, PNG_SIGNATURE_SIZE> signature = {0};
  int isSignatureValid = 0;  // Результат проверки сигнатуры.

  // Считываем сигнатуру (первые PNG_SIGNATURE_SIZE байт).
  source.read((s8_t *)signature.data(), PNG_SIGNATURE_SIZE);

  // Проверяем на соответствие считанной нами сигнатуры с сигнатурой PNG-формата.
  isSignatureValid = png_sig_cmp(signature.data(), 0, PNG_SIGNATURE_SIZE);
  if (isSignatureValid != 0) {
    source.close();
  }
}

void PNGPlugin::create_(std::ifstream &source) {
  // Выделяем память и инициализируем структуру для работы с файлом.
  png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGPlugin::error, PNGPlugin::warning);
  if (!png_) {
    source.close();
  }

  // Выделяем память и инициализируем структуру с информацией о файле.
  info_ = png_create_info_struct(png_);
  if (!info_) {
    png_destroy_read_struct(&png_, (png_infopp) nullptr, (png_infopp) nullptr);
    source.close();
  }

  // Выделяем память и инициализируем структуру с информацией о файле.
  endInfo_ = png_create_info_struct(png_);
  if (!endInfo_) {
    png_destroy_read_struct(&png_, &info_, (png_infopp) nullptr);
    source.close();
  }
}

auto PNGPlugin::loadFromStream(std::ifstream &source) -> ImageDescriptor {
  readSignature_(source);
  create_(source);

  // Устанавливаем механизм обработки ошибок.
  if (setjmp(png_jmpbuf(png_))) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
    source.close();
  }

  png_set_read_fn(png_, (png_voidp)&source, PNGPlugin::readData);  // Устанавливаем собственную функцию чтения данных.
  png_set_sig_bytes(png_,
      PNG_SIGNATURE_SIZE);  // Сообщаем Libpng, что мы уже прочитали PNG_SIGNATURE_SIZE байт, когда проверяли сигнатуру.
  png_read_info(png_, info_);  // Читаем информацию о данных изображения.

  u32_t width = png_get_image_width(png_, info_);  // Ширина изображения в пикселях.
  u32_t height = png_get_image_height(png_, info_);  // Высота изображения в пикселях.

  int bitDepth = png_get_bit_depth(png_, info_);  // Глубина цвета.
  int colorType = png_get_color_type(png_, info_);
  // int interlaceMethod = png_get_interlace_type(png_, info_);
  // int compressionMethod = png_get_compression_type(png_, info_);
  // int filterMethod = png_get_filter_type(png_, info_);
  int channels = png_get_channels(png_, info_);

  switch (colorType) {
    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(png_);
      png_set_filler(png_, 0xff, PNG_FILLER_AFTER);
      break;
    case PNG_COLOR_TYPE_GRAY:
      png_set_filler(png_, 0xff, PNG_FILLER_AFTER);
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      if (bitDepth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_);
      }
      if (bitDepth == 16) {
        png_set_strip_16(png_);
      }
      png_set_gray_to_rgb(png_);
      break;
    case PNG_COLOR_TYPE_RGB:
      if (bitDepth < 8) {
        png_set_packing(png_);
      }
      if (bitDepth == 16) {
        png_set_strip_16(png_);
      }
      png_set_filler(png_, 0xff, PNG_FILLER_AFTER);
      break;
    default:
      break;
  }

  if (png_get_valid(png_, info_, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_);
    channels += 1;
  }

  png_read_update_info(png_, info_);  // Обновляем информацию структуры png.

  // Получаем кол-во байтов необходимых для вмещения преобразованного ряда.
  int rowBytes = png_get_rowbytes(png_, info_);

  // Выделяем памяти под данные изображения.
  auto *tempData = (u8_t *)malloc(rowBytes * height);
  if (!tempData) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
    free(tempData);
    source.close();
  }

  // Выделяем память под указатели на каждую строку.
  png_bytep *rowPointers = new png_bytep[height];
  if (!rowPointers) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
    free(tempData);
    SAFE_DELETE_ARRAY(rowPointers);
    source.close();
  }

  for (u32_t i = 0; i < height; ++i) {
    rowPointers[i] = &tempData[i * rowBytes];
  }

  png_read_image(png_, rowPointers);  // Читаем изображение.
  png_read_end(png_, endInfo_);

  ImageDescriptor descriptor = {};
  descriptor.pixels = tempData;
  descriptor.width = width;
  descriptor.height = height;
  descriptor.bpp = channels * bitDepth;
  // descriptor.type = (bitDepth == 16) ? UInt16 : UByte;
  // descriptor.internalFormat = RGBA8;
  // descriptor.format = RGBA;

  png_destroy_read_struct(&png_, &info_, &endInfo_);  // Освобождаем память выделенную для библиотеки Libpng.
  SAFE_DELETE_ARRAY(rowPointers);  // Освобождаем память от указателей на строки.
  source.close();

  return descriptor;
}

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)
