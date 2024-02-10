#include <sway/loader/png/pngplugin.hpp>

#include <iostream>

#define PNG_SIGNATURE_SIZE 8  // Длина сигнатуры.
#define PNG_INFOPP_NULL (png_infopp) nullptr

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

// clang-format off
DECLARE_LOADER_PLUGIN(PNGPlugin, "png", 
  "Victor Timoshin", 
  "PNG Loader", 
  "https://github.com/timcogames/sway.module_loader-png",
  "MIT", 
  core::Version(1))
// clang-format on

void PNGPlugin::readData(png_structp png, png_bytep data, png_size_t length) {
  reinterpret_cast<std::istream *>(png_get_io_ptr(png))->read((lpstr_t)data, length);
}

void PNGPlugin::readAsyncData(png_structp png, png_bytep data, png_size_t length) {
  auto *source = (ImageSource *)png_get_io_ptr(png);
  if ((int)(source->offset + length) <= source->size) {
    memcpy(data, source->data + source->offset, length);
    source->offset += length;
  } else {
    png_error(png, "readAsyncData failed");
  }
}

auto PNGPlugin::readSignature_nostream_(void *buffer) -> bool {
  if (buffer == nullptr) {
    return false;
  }

  std::array<std::byte, PNG_SIGNATURE_SIZE> signature;
  const auto successed = png_sig_cmp(reinterpret_cast<png_bytep>(buffer), (png_size_t)0, PNG_SIGNATURE_SIZE);
  if (successed != 0) {
    return false;
  }

  return true;
}

void PNGPlugin::readSignature_(std::ifstream &source) {
  std::array<std::byte, PNG_SIGNATURE_SIZE> signature;
  s32_t successed = 0;  // Результат проверки сигнатуры.

  // Считываем сигнатуру (первые PNG_SIGNATURE_SIZE байт).
  source.read((s8_t *)signature.data(), PNG_SIGNATURE_SIZE);

  // Проверяем на соответствие считанной нами сигнатуры с сигнатурой PNG-формата.
  successed = png_sig_cmp(reinterpret_cast<png_bytep>(signature.data()), 0, PNG_SIGNATURE_SIZE);
  if (successed != 0) {
    source.close();
  }
}

// Выделяем память и инициализируем структуру с информацией о файле.
auto PNGPlugin::createInfoStruct_nostream_() -> png_infop {
  auto *infoPtr = png_create_info_struct(png_);
  if (infoPtr == nullptr) {
    png_destroy_read_struct(&png_, PNG_INFOPP_NULL, PNG_INFOPP_NULL);
  }

  return infoPtr;
}

// Выделяем память и инициализируем структуру с информацией о файле.
auto PNGPlugin::createInfoStruct_(std::ifstream &source) -> png_infop {
  auto *infoPtr = png_create_info_struct(png_);
  if (infoPtr == nullptr) {
    png_destroy_read_struct(&png_, PNG_INFOPP_NULL, PNG_INFOPP_NULL);
    source.close();
  }

  return infoPtr;
}

void PNGPlugin::create_nostream_() {
  // Выделяем память и инициализируем структуру для работы с файлом.
  png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGPlugin::error, PNGPlugin::warning);
  if (png_ == nullptr) {
    printf("FAILED png_create_read_struct\n");
  }

  info_ = createInfoStruct_nostream_();
  endInfo_ = png_create_info_struct(png_);
  if (endInfo_ == nullptr) {
    png_destroy_read_struct(&png_, &info_, PNG_INFOPP_NULL);
  }
}

void PNGPlugin::create_(std::ifstream &source) {
  // Выделяем память и инициализируем структуру для работы с файлом.
  png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGPlugin::error, PNGPlugin::warning);
  if (png_ == nullptr) {
    source.close();
  }

  info_ = createInfoStruct_(source);
  endInfo_ = png_create_info_struct(png_);
  if (endInfo_ == nullptr) {
    png_destroy_read_struct(&png_, &info_, PNG_INFOPP_NULL);
    source.close();
  }
}

void PNGPlugin::getImageSizeInfo_(math::size2i_t &size) {
  size.setW(png_get_image_width(png_, info_));  // Ширина изображения в пикселях.
  size.setH(png_get_image_height(png_, info_));  // Высота изображения в пикселях.
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

  // Сообщаем Libpng, что мы уже прочитали PNG_SIGNATURE_SIZE байт, когда проверяли сигнатуру.
  png_set_sig_bytes(png_, PNG_SIGNATURE_SIZE);
  png_read_info(png_, info_);  // Читаем информацию о данных изображения.

  math::size2i_t size;
  getImageSizeInfo_(size);

  const int bitDepth = png_get_bit_depth(png_, info_);  // Глубина цвета.
  const int colorType = png_get_color_type(png_, info_);
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

  // Получаем кол.-во байтов необходимых для вмещения преобразованного ряда.
  const int rowBytes = png_get_rowbytes(png_, info_);

  auto imgBufLen = rowBytes * size.getH();
  // Выделяем память под данные изображения.
  auto *imgBufData = (u8_t *)malloc(imgBufLen);
  if (imgBufData == nullptr) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
    free(imgBufData);
    source.close();
  }

  // Выделяем память под указатели на каждую строку.
  auto *rowPointers = new png_bytep[size.getH()];
  if (rowPointers == nullptr) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
    free(imgBufData);
    SAFE_DELETE_ARRAY(rowPointers);
    source.close();
  }

  for (auto row = 0; row < size.getH(); ++row) {
    rowPointers[row] = &imgBufData[row * rowBytes];
  }

  png_read_image(png_, rowPointers);  // Читаем изображение.
  png_read_end(png_, endInfo_);

  ImageDescriptor descriptor = {};
  descriptor.buf.data = imgBufData;
  descriptor.buf.len = imgBufLen;
  descriptor.size = size;
  descriptor.bpp = channels * bitDepth;
  // descriptor.type = (bitDepth == 16) ? UInt16 : UByte;
  // descriptor.internalFormat = RGBA8;
  // descriptor.format = RGBA;

  png_destroy_read_struct(&png_, &info_, &endInfo_);  // Освобождаем память выделенную для библиотеки Libpng.
  SAFE_DELETE_ARRAY(rowPointers);  // Освобождаем память от указателей на строки.
  source.close();

  return descriptor;
}

auto PNGPlugin::loadFrom(void *buffer, int size_tmp) -> ImageDescriptor {
  printf("PNG SIZE: %i\n", size_tmp);
  readSignature_nostream_(buffer);
  create_nostream_();

  // Устанавливаем механизм обработки ошибок.
  if (setjmp(png_jmpbuf(png_))) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
  }

  ImageSource imageSource;
  imageSource.data = (unsigned char *)buffer;
  imageSource.size = size_tmp;
  imageSource.offset = 0;

  png_set_read_fn(png_, &imageSource, PNGPlugin::readAsyncData);  // Устанавливаем собственную функцию чтения данных.

  // Сообщаем Libpng, что мы уже прочитали PNG_SIGNATURE_SIZE байт, когда проверяли сигнатуру.
  // png_set_sig_bytes(png_, PNG_SIGNATURE_SIZE); // libpng error: PNG unsigned integer out of range
  png_set_sig_bytes(png_, 0);
  png_read_info(png_, info_);  // Читаем информацию о данных изображения.

  math::size2i_t size;
  getImageSizeInfo_(size);

  const int bitDepth = png_get_bit_depth(png_, info_);  // Глубина цвета.
  const int colorType = png_get_color_type(png_, info_);
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

  // Получаем кол.-во байтов необходимых для вмещения преобразованного ряда.
  const auto rowBytes = png_get_rowbytes(png_, info_);

  auto imgBufLen = rowBytes * size.getH();
  // Выделяем память под данные изображения.
  auto *imgBufData = (u8_t *)malloc(imgBufLen);
  if (imgBufData == nullptr) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
    free(imgBufData);
  }

  // Выделяем память под указатели на каждую строку.
  auto *rowPointers = new png_bytep[size.getH()];
  if (rowPointers == nullptr) {
    png_destroy_read_struct(&png_, &info_, &endInfo_);
    free(imgBufData);
    SAFE_DELETE_ARRAY(rowPointers);
  }

  for (auto row = 0; row < size.getH(); ++row) {
    rowPointers[row] = &imgBufData[row * rowBytes];
  }

  png_read_image(png_, rowPointers);  // Читаем изображение.
  png_read_end(png_, endInfo_);

  ImageDescriptor descriptor = {};
  descriptor.buf.data = imgBufData;
  descriptor.buf.len = imgBufLen;
  descriptor.size = size;
  descriptor.bpp = channels * bitDepth;
  // descriptor.type = (bitDepth == 16) ? UInt16 : UByte;
  // descriptor.internalFormat = RGBA8;
  // descriptor.format = RGBA;

  png_destroy_read_struct(&png_, &info_, &endInfo_);  // Освобождаем память выделенную для библиотеки Libpng.
  SAFE_DELETE_ARRAY(rowPointers);  // Освобождаем память от указателей на строки.

  return descriptor;
}

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)
