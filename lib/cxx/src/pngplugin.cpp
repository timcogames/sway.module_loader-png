#include <sway/loader/png/defines.hpp>
#include <sway/loader/png/palette.hpp>
#include <sway/loader/png/pngplugin.hpp>

#include <fstream>
#include <unordered_set>

using namespace sway::core;

namespace sway::loader::png {

EMSCRIPTEN_BINDING_BEGIN(PNGPlugin)
#if (defined EMSCRIPTEN_PLATFORM && defined EMSCRIPTEN_USE_BINDINGS)
emscripten::class_<PNGPlugin>("PNGPlugin")
    .constructor<>()
    // .function("loadFromStream", &PNGPlugin::loadFromStream);
    .function("loadFromArrayBuffer", &PNGPlugin::loadFromArrayBuffer)
    .function("saveToIndexedArrayBuffer", &PNGPlugin::saveToIndexedArrayBuffer)
    .function("setCustomPalette", &PNGPlugin::setCustomPalette)
    .function("getImageDescriptor", &PNGPlugin::getImageDescriptor);
#endif
EMSCRIPTEN_BINDING_END()

// clang-format off
DECLARE_LOADER_PLUGIN(PNGPlugin, "png", 
  "Victor Timoshin", 
  "PNG Loader", 
  "https://github.com/timcogames/sway.module_loader-png",
  "MIT", 
  Version(1));
// clang-format on

void PNGPlugin::readData(png_structp png, png_bytep data, png_size_t length) {
  reinterpret_cast<std::istream *>(png_get_io_ptr(png))->read((lpstr_t)data, length);
}

void PNGPlugin::readAsyncData(png_structp png, png_bytep data, png_size_t length) {
  auto *source = (ImageSource *)png_get_io_ptr(png);
  if ((int)(source->offset + length) <= source->size) {
#ifdef EMSCRIPTEN_PLATFORM
    emscripten::val sourceData = source->data;
    emscripten::val subArray = sourceData.call<emscripten::val>("subarray", source->offset, source->offset + length);

    // Копируем данные в целевой буфер
    emscripten::val targetView = emscripten::val(emscripten::typed_memory_view(length, data));
    targetView.call<void>("set", subArray);
#else
    memcpy(data, source->data + source->offset, length);
#endif

    source->offset += length;
  } else {
    png_error(png, "readAsyncData failed");
  }
}

void PNGPlugin::writeData(png_structp png, png_bytep data, png_size_t length) {
  std::vector<u8_t> *output = reinterpret_cast<std::vector<u8_t> *>(png_get_io_ptr(png));
  output->insert(output->end(), data, data + length);
}

void PNGPlugin::flushData(png_structp png) {
  // Empty
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
  i32_t successed = 0;  // Результат проверки сигнатуры.

  // Считываем сигнатуру (первые PNG_SIGNATURE_SIZE байт).
  source.read((lpstr_t)signature.data(), PNG_SIGNATURE_SIZE);

  // Проверяем на соответствие считанной нами сигнатуры с сигнатурой PNG-формата.
  successed = png_sig_cmp(reinterpret_cast<png_bytep>(signature.data()), 0, PNG_SIGNATURE_SIZE);
  if (successed != 0) {
    source.close();
  }
}

// Выделяем память и инициализируем структуру с информацией о файле.
auto PNGPlugin::createInfoStruct_nostream_() -> png_infop {
  auto *infoPtr = png_create_info_struct(reader_.png);
  if (infoPtr == nullptr) {
    png_destroy_read_struct(&reader_.png, PNG_INFOPP_NULL, PNG_INFOPP_NULL);
  }

  return infoPtr;
}

// Выделяем память и инициализируем структуру с информацией о файле.
auto PNGPlugin::createInfoStruct_(std::ifstream &source) -> png_infop {
  auto *infoPtr = png_create_info_struct(reader_.png);
  if (infoPtr == nullptr) {
    png_destroy_read_struct(&reader_.png, PNG_INFOPP_NULL, PNG_INFOPP_NULL);
    source.close();
  }

  return infoPtr;
}

void PNGPlugin::create_nostream_() {
  // Выделяем память и инициализируем структуру для работы с файлом.
  reader_.png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGPlugin::error, PNGPlugin::warning);
  if (reader_.png == nullptr) {
    printf("FAILED png_create_read_struct\n");
  }

  reader_.info = createInfoStruct_nostream_();
  endInfo_ = png_create_info_struct(reader_.png);
  if (endInfo_ == nullptr) {
    png_destroy_read_struct(&reader_.png, &reader_.info, PNG_INFOPP_NULL);
  }
}

void PNGPlugin::create_(std::ifstream &source) {
  // Выделяем память и инициализируем структуру для работы с файлом.
  reader_.png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGPlugin::error, PNGPlugin::warning);
  if (reader_.png == nullptr) {
    source.close();
  }

  reader_.info = createInfoStruct_(source);
  endInfo_ = png_create_info_struct(reader_.png);
  if (endInfo_ == nullptr) {
    png_destroy_read_struct(&reader_.png, &reader_.info, PNG_INFOPP_NULL);
    source.close();
  }
}

void PNGPlugin::getImageSizeInfo_(math::size2i_t &size) {
  size.setW(png_get_image_width(reader_.png, reader_.info));  //!< Ширина изображения в пикселях.
  size.setH(png_get_image_height(reader_.png, reader_.info));  //!< Высота изображения в пикселях.
}

void PNGPlugin::transformation_(png_byte colorType, png_byte bitDepth, png_byte *channels, png_byte *bitsPerChannel) {

  switch (colorType) {
    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(reader_.png);
      png_set_filler(reader_.png, 0xff, PNG_FILLER_AFTER);
      *channels = 4;  //!< После конвертации в RGBA.
      break;
    case PNG_COLOR_TYPE_GRAY:
      png_set_filler(reader_.png, 0xff, PNG_FILLER_AFTER);
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      if (bitDepth < 8) {
        png_set_expand_gray_1_2_4_to_8(reader_.png);
        *bitsPerChannel = 8;  //!< После расширения.
      }
      if (bitDepth == 16) {
        png_set_strip_16(reader_.png);
        *bitsPerChannel = 8;  //!< После уменьшения глубины.
      }
      png_set_gray_to_rgb(reader_.png);
      *channels = 4;  //!< После конвертации в RGBA.
      break;
    case PNG_COLOR_TYPE_RGB:
      if (bitDepth < 8) {
        png_set_packing(reader_.png);
        *bitsPerChannel = 8;  //!< После упаковки.
      }
      if (bitDepth == 16) {
        png_set_strip_16(reader_.png);
        *bitsPerChannel = 8;  //!< После уменьшения глубины.
      }
      png_set_filler(reader_.png, 0xff, PNG_FILLER_AFTER);
      *channels = 4;  //!< После добавления альфа-канала.
      break;
    case PNG_COLOR_TYPE_RGBA:
      if (bitDepth == 16) {
        png_set_strip_16(reader_.png);
        *bitsPerChannel = 8;
      }
      break;
    default:
      break;
  }

  if (png_get_valid(reader_.png, reader_.info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(reader_.png);
    *channels += 1;
  }

  png_read_update_info(reader_.png, reader_.info);  //!< Обновляем информацию структуры png.
}

struct ColorHash {
  std::size_t operator()(const math::Color<u8_t> &color) const {
    std::hash<u32_t> result;
    // Упаковываем RGBA в 32-битное число
    uint32_t value = (color.getR() << 24) | (color.getG() << 16) | (color.getB() << 8) | color.getA();
    return result(value);
  }
};

struct ColorEqual {
  bool operator()(const math::Color<u8_t> &a, const math::Color<u8_t> &b) const { return a == b; }
};

void PNGPlugin::analyzeColors_(ImageDescriptor &descriptor, u8_t *imageData, const math::size2i_t size, int channels) {
  std::unordered_set<math::Color<u8_t>, ColorHash, ColorEqual> uniqueColors;
  const auto totalPixels = size.getW() * size.getH();

  for (auto y = 0; y < size.getH(); ++y) {
    for (auto x = 0; x < size.getW(); ++x) {
      const auto index = (y * size.getW() + x) * channels;

      math::Color<u8_t> color;
      color.set(imageData[index], imageData[index + 1], imageData[index + 2],
          (channels >= 4) ? imageData[index + 3] : 255 /* Полностью непрозрачный */);

      uniqueColors.insert(color);
    }
  }

  descriptor.colorAnalysis.uniqueColors.assign(uniqueColors.begin(), uniqueColors.end());
  descriptor.colorAnalysis.totalPixels = totalPixels;
}

auto PNGPlugin::loadFromStream(std::ifstream &source) -> ImageDescriptor {
  ImageDescriptor descriptor = {};

  readSignature_(source);
  create_(source);

  // Устанавливаем механизм обработки ошибок.
  if (setjmp(png_jmpbuf(reader_.png))) {
    png_destroy_read_struct(&reader_.png, &reader_.info, &endInfo_);
    source.close();
  }

  png_set_read_fn(
      reader_.png, (png_voidp)&source, PNGPlugin::readData);  //!< Устанавливаем собственную функцию чтения данных.

  // Сообщаем Libpng, что мы уже прочитали PNG_SIGNATURE_SIZE байт, когда проверяли сигнатуру.
  png_set_sig_bytes(reader_.png, PNG_SIGNATURE_SIZE);
  png_read_info(reader_.png, reader_.info);  //!< Читаем информацию о данных изображения.

  math::size2i_t size;
  getImageSizeInfo_(size);

  PaletteUtil::get(reader_, descriptor.palette);

  const png_byte colorType = png_get_color_type(reader_.png, reader_.info);
  const png_byte bitDepth = png_get_bit_depth(reader_.png, reader_.info);  //!< Глубина цвета.
  // int interlaceMethod = png_get_interlace_type(reader_.png, reader_.info);
  // int compressionMethod = png_get_compression_type(reader_.png, reader_.info);
  // int filterMethod = png_get_filter_type(reader_.png, reader_.info);
  png_byte channels = png_get_channels(reader_.png, reader_.info);

  // Определяем bits per channel.
  png_byte bitsPerChannel = bitDepth;

  // Для indexed цветов определяем реальную глубину.
  if (colorType == PNG_COLOR_TYPE_PALETTE) {
    // Для палитры bits per channel - это глубина исходных данных.
    bitsPerChannel = bitDepth;
  }

  transformation_(colorType, bitDepth, &channels, &bitsPerChannel);

  // Получаем кол.-во байтов необходимых для вмещения преобразованного ряда.
  const int rowBytes = png_get_rowbytes(reader_.png, reader_.info);

  auto imgBufLen = rowBytes * size.getH();
  // Выделяем память под данные изображения.
  auto *imgBufData = (u8_t *)malloc(imgBufLen);
  if (imgBufData == nullptr) {
    png_destroy_read_struct(&reader_.png, &reader_.info, &endInfo_);
    free(imgBufData);
    source.close();
  }

  // Выделяем память под указатели на каждую строку.
  auto *rowPointers = new png_bytep[size.getH()];
  if (rowPointers == nullptr) {
    png_destroy_read_struct(&reader_.png, &reader_.info, &endInfo_);
    free(imgBufData);
    safeDeleteArray<png_byte *>(rowPointers);
    source.close();
  }

  for (auto row = 0; row < size.getH(); ++row) {
    rowPointers[row] = &imgBufData[row * rowBytes];
  }

  png_read_image(reader_.png, rowPointers);  //!< Читаем изображение.
  png_read_end(reader_.png, endInfo_);

  analyzeColors_(descriptor, imgBufData, size, channels);

#ifdef EMSCRIPTEN_PLATFORM
  descriptor.buf.data = emscripten::val(emscripten::typed_memory_view(imgBufLen, imgBufData));
#else
  descriptor.buf.data = imgBufData;
#endif

  descriptor.buf.len = imgBufLen;
  descriptor.size = size;
  descriptor.bpp = channels * bitsPerChannel;
  descriptor.bitsPerChannel = bitsPerChannel;
  descriptor.channels = channels;
  descriptor.colorsPerPalette = descriptor.palette.colors.size();
  descriptor.paletteCount = descriptor.palette.colors.size() > 0 ? 1 : 0;  //!< PNG обычно имеет одну палитру.
  // descriptor.colorType = actualColorType;
  // descriptor.bitDepth = actualBitDepth;

  png_destroy_read_struct(
      &reader_.png, &reader_.info, &endInfo_);  //!< Освобождаем память выделенную для библиотеки Libpng.
  safeDeleteArray<png_byte *>(rowPointers);  //!< Освобождаем память от указателей на строки.
  source.close();

  return descriptor;
}

auto PNGPlugin::loadFrom(void *buffer, int size_tmp) -> ImageDescriptor {
  ImageDescriptor descriptor = {};

  readSignature_nostream_(buffer);
  create_nostream_();

  // Устанавливаем механизм обработки ошибок.
  if (setjmp(png_jmpbuf(reader_.png))) {
    png_destroy_read_struct(&reader_.png, &reader_.info, &endInfo_);
  }

  ImageSource imageSource = {};
#ifdef EMSCRIPTEN_PLATFORM
  imageSource.data = emscripten::val(emscripten::typed_memory_view(size_tmp, static_cast<uint8_t *>(buffer)));
#else
  imageSource.data = static_cast<u8_t *>(buffer);
#endif
  imageSource.size = size_tmp;
  imageSource.offset = 0;

  png_set_read_fn(
      reader_.png, &imageSource, PNGPlugin::readAsyncData);  //!< Устанавливаем собственную функцию чтения данных.

  // Сообщаем Libpng, что мы уже прочитали PNG_SIGNATURE_SIZE байт, когда проверяли сигнатуру.
  // png_set_sig_bytes(reader_.png, PNG_SIGNATURE_SIZE); // libpng error: PNG unsigned integer out of range
  png_set_sig_bytes(reader_.png, 0);
  png_read_info(reader_.png, reader_.info);  //!< Читаем информацию о данных изображения.

  math::size2i_t size;
  getImageSizeInfo_(size);

  PaletteUtil::get(reader_, descriptor.palette);

  const png_byte bitDepth = png_get_bit_depth(reader_.png, reader_.info);  //!< Глубина цвета.
  const png_byte colorType = png_get_color_type(reader_.png, reader_.info);
  // int interlaceMethod = png_get_interlace_type(reader_.png, reader_.info);
  // int compressionMethod = png_get_compression_type(reader_.png, reader_.info);
  // int filterMethod = png_get_filter_type(reader_.png, reader_.info);
  png_byte channels = png_get_channels(reader_.png, reader_.info);

  // Определяем bits per channel.
  png_byte bitsPerChannel = bitDepth;

  // Для indexed цветов определяем реальную глубину.
  if (colorType == PNG_COLOR_TYPE_PALETTE) {
    // Для палитры bits per channel - это глубина исходных данных.
    bitsPerChannel = bitDepth;
  }

  // transformation_(colorType, bitDepth, &channels, &bitsPerChannel);

  // Получаем актуальную информацию после преобразований.
  const int actualBitDepth = png_get_bit_depth(reader_.png, reader_.info);
  const int actualColorType = png_get_color_type(reader_.png, reader_.info);
  const int actualChannels = png_get_channels(reader_.png, reader_.info);

  // Обновляем bitsPerChannel на основе актуальных данных.
  bitsPerChannel = actualBitDepth;

  // Получаем кол.-во байтов необходимых для вмещения преобразованного ряда.
  const auto rowBytes = png_get_rowbytes(reader_.png, reader_.info);

  auto imgBufLen = rowBytes * size.getH();
  // Выделяем память под данные изображения.
  auto *imgBufData = (u8_t *)malloc(imgBufLen);
  if (imgBufData == nullptr) {
    png_destroy_read_struct(&reader_.png, &reader_.info, &endInfo_);
    free(imgBufData);
  }

  // Выделяем память под указатели на каждую строку.
  auto *rowPointers = new png_bytep[size.getH()];
  if (rowPointers == nullptr) {
    png_destroy_read_struct(&reader_.png, &reader_.info, &endInfo_);
    free(imgBufData);
    safeDeleteArray<png_byte *>(rowPointers);
  }

  for (auto row = 0; row < size.getH(); ++row) {
    rowPointers[row] = &imgBufData[row * rowBytes];
  }

  png_read_image(reader_.png, rowPointers);  //!< Читаем изображение.
  png_read_end(reader_.png, endInfo_);

  analyzeColors_(descriptor, imgBufData, size, actualChannels);

#ifdef EMSCRIPTEN_PLATFORM
  descriptor.buf.data = emscripten::val(emscripten::typed_memory_view(imgBufLen, imgBufData));
#else
  descriptor.buf.data = imgBufData;
#endif
  descriptor.buf.len = imgBufLen;
  descriptor.size = size;
  descriptor.bpp = channels * bitsPerChannel;
  // descriptor.type = (bitDepth == 16) ? UInt16 : UByte;
  // descriptor.internalFormat = RGBA8;
  // descriptor.format = RGBA;
  descriptor.bitsPerChannel = bitsPerChannel;
  descriptor.channels = channels;
  descriptor.colorsPerPalette = descriptor.palette.colors.size();
  descriptor.paletteCount = descriptor.palette.colors.size() > 0 ? 1 : 0;  //!< PNG обычно имеет одну палитру.
  descriptor.colorType = actualColorType;
  descriptor.bitDepth = actualBitDepth;

  png_destroy_read_struct(
      &reader_.png, &reader_.info, &endInfo_);  //!< Освобождаем память выделенную для библиотеки Libpng.
  safeDeleteArray<png_byte *>(rowPointers);  //!< Освобождаем память от указателей на строки.

  return descriptor;
}

// Функция для вычисления цветового расстояния (дельта E)
double colorDistance(const math::Color<u8_t> &color1, const math::Color<u8_t> &color2) {
  double dr = color1.getR() - color2.getR();
  double dg = color1.getG() - color2.getG();
  double db = color1.getB() - color2.getB();
  return std::sqrt(dr * dr + dg * dg + db * db);
}

void getImageData(const ImageDescriptor &descriptor, u8_t **imageData, size_t *imageDataSize) {
  std::vector<u8_t> imageDataVector;

#ifdef EMSCRIPTEN_PLATFORM
  emscripten::val imageDataVal = descriptor.buf.data;
  if (imageDataVal.isNull() || imageDataVal == emscripten::val::undefined()) {
    return;
  }

  emscripten::val uint8Array = emscripten::val::global("Uint8Array").new_(imageDataVal);
  *imageDataSize = uint8Array["length"].as<size_t>();

  imageDataVector.resize(*imageDataSize);
  emscripten::val memoryView = emscripten::val(emscripten::typed_memory_view(*imageDataSize, imageDataVector.data()));
  memoryView.call<void>("set", uint8Array);
  *imageData = imageDataVector.data();

  printf("Loaded image data: %zu bytes, channels: %d\n", *imageDataSize, descriptor.channels);
#else
  *imageData = descriptor.buf.data;
  *imageDataSize = descriptor.buf.len;
#endif
}

void createIndexedData(ImageDescriptor &descriptor, const std::vector<math::Color<u8_t>> &customPalette) {
  const auto width = descriptor.size.getW();
  const auto height = descriptor.size.getH();

  // Создаем indexed данные.
  std::vector<u8_t> indexedData(width * height);

  // Получаем доступ к оригинальным данным изображения.
  u8_t *imageData = nullptr;
  size_t imageDataSize = 0;
  getImageData(descriptor, &imageData, &imageDataSize);
  if (imageData == nullptr) {
    printf("Failed to get image data for indexing\n");
    descriptor.indexed.data = indexedData;
    descriptor.indexed.size = indexedData.size();
    return;
  }

  int channels = descriptor.channels;
  printf("Creating indexed data for Sega MD: %dx%d, channels: %d, data size: %zu\n", width, height, channels,
      imageDataSize);

  // Ограничиваем палитру 16 цветами для Sega MD.
  std::vector<math::Color<u8_t>> limitedPalette = customPalette;
  if (limitedPalette.size() > PNG_PALETTE_SIZE) {
    printf("Limiting palette from %zu to %zu colors for Sega MD\n", limitedPalette.size(), PNG_PALETTE_SIZE);
    limitedPalette.resize(PNG_PALETTE_SIZE);
  }

  // Определяем индекс прозрачного цвета в палитре.
  i32_t transparentIndex = PaletteUtil::findTransparentIndex(limitedPalette);

  // Обрабатываем каждый пиксель.
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int srcIndex = (y * width + x) * channels;

      // Проверяем границы массива.
      if (srcIndex >= imageDataSize) {
        indexedData[y * width + x] = (transparentIndex != -1) ? transparentIndex : 0;
        continue;
      }

      u8_t r, g, b, a = PNG_COLOR_CHANNEL_MAX;

      if (channels >= 4) {
        // RGBA формат - проверяем альфа-канал.
        r = imageData[srcIndex];
        g = imageData[srcIndex + 1];
        b = imageData[srcIndex + 2];
        a = imageData[srcIndex + 3];

        // Если пиксель прозрачный (альфа < 128), используем прозрачный цвет.
        if (a < 128) {
          indexedData[y * width + x] = transparentIndex;
          continue;
        }
      } else if (channels >= 3) {
        // RGB формат.
        r = imageData[srcIndex];
        g = imageData[srcIndex + 1];
        b = imageData[srcIndex + 2];
      } else if (channels == 1) {
        // Градации серого - преобразуем в RGB.
        u8_t gray = imageData[srcIndex];
        r = gray;
        g = gray;
        b = gray;
      } else {
        r = g = b = PNG_COLOR_CHANNEL_MIN;
      }

      // Если это прозрачный цвет (маджента), используем прозрачный индекс.
      if (transparentIndex != -1 && PaletteUtil::hasMagenta(math::Color<u8_t>(r, g, b, a))) {
        indexedData[y * width + x] = transparentIndex;
        continue;
      }

      // Ищем ближайший цвет в ограниченной палитре.
      u8_t bestIndex = 0;
      double minDistance = std::numeric_limits<double>::max();

      for (size_t i = 0; i < limitedPalette.size(); ++i) {
        // // Пропускаем прозрачный цвет при поиске ближайшего.
        // if (i == transparentIndex) {
        //   continue;
        // }

        const auto &paletteColor = limitedPalette[i];
        // Сравниваем цвета без учета альфа-канала для поиска ближайшего.
        const auto distance = colorDistance(math::Color<u8_t>(r, g, b, PNG_COLOR_CHANNEL_MAX), paletteColor);

        if (distance < minDistance) {
          minDistance = distance;
          bestIndex = static_cast<u8_t>(i);
        }
      }

      // Ограничиваем индекс 4 битами (0-15).
      indexedData[y * width + x] = bestIndex & 0x0F;
    }
  }

  // Сохраняем indexed данные в дескрипторе.
  descriptor.indexed.data = indexedData;
  descriptor.indexed.size = indexedData.size();
  printf("Sega MD indexed data created: %zu bytes, palette: %zu colors, transparent index: %d\n", indexedData.size(),
      limitedPalette.size(), transparentIndex);
}

#ifdef EMSCRIPTEN_PLATFORM

auto PNGPlugin::loadFromArrayBuffer(emscripten::val buf) -> ImageDescriptor {
  if (buf.isNull() || buf == emscripten::val::undefined()) {
    return ImageDescriptor{};
  }

  unsigned int length = buf["byteLength"].as<unsigned int>();

  // Создаем временный буфер для работы с libpng.
  std::vector<uint8_t> tempBuffer(length);
  emscripten::val uint8Array = emscripten::val::global("Uint8Array").new_(buf);
  emscripten::val memoryView = emscripten::val(emscripten::typed_memory_view(length, tempBuffer.data()));
  memoryView.call<void>("set", uint8Array);

  // Используем существующий метод loadFrom с временным буфером.
  return loadFrom(tempBuffer.data(), length);
}

void PNGPlugin::setCustomPalette(ImageDescriptor &desc, const emscripten::val &colors) {
  printf("Setting custom palette...\n");

  PaletteUtil::setCustom(desc, colors);
  printf("Custom palette set with %zu colors\n", desc.palette.colors.size());

  // Создаем indexed данные на основе пользовательской палитры
  createIndexedData(desc, desc.palette.colors);

  // Добавляем отладочную информацию
  printf("After createIndexedData - Palette size: %zu\n", desc.palette.colors.size());
  printf("After createIndexedData - Indexed data size: %zu\n", desc.indexed.data.size());
  printf("After createIndexedData - Image size: %dx%d\n", desc.size.getW(), desc.size.getH());

  descriptor_ = desc;
  printf("Custom palette and indexed data setup completed\n");
}

auto PNGPlugin::getImageDescriptor() -> ImageDescriptor { return descriptor_; }

auto PNGPlugin::saveToIndexedArrayBuffer([[maybe_unused]] ImageDescriptor &desc) -> emscripten::val {
  // ВАЖНО: используем внутренний descriptor_ вместо переданного desc
  ImageDescriptor &descriptorToSave = descriptor_;

  // Проверяем, что у нас есть indexed данные
  if (descriptorToSave.indexed.data.empty()) {
    printf("No indexed data available! Palette size: %zu, Image size: %dx%d\n", descriptorToSave.palette.colors.size(),
        descriptorToSave.size.getW(), descriptorToSave.size.getH());
    return emscripten::val::null();
  }

  // Проверяем, что палитра не пустая
  if (descriptorToSave.palette.colors.empty()) {
    printf("Palette is empty!\n");
    return emscripten::val::null();
  }

  printf("Saving indexed PNG for SGDK: %dx%d, palette: %zu colors, indexed data: %zu bytes\n",
      descriptorToSave.size.getW(), descriptorToSave.size.getH(), descriptorToSave.palette.colors.size(),
      descriptorToSave.indexed.data.size());

  writer_.png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (writer_.png == nullptr) {
    printf("Failed to create PNG write structure\n");
    return emscripten::val::null();
  }

  writer_.info = png_create_info_struct(writer_.png);
  if (writer_.info == NULL) {
    printf("Failed to create PNG info structure");
    png_destroy_write_struct(&writer_.png, nullptr);
    return emscripten::val::null();
  }

  if (setjmp(png_jmpbuf(writer_.png))) {
    printf("Error during PNG creation");
    png_destroy_write_struct(&writer_.png, &writer_.info);
    return emscripten::val::null();
  }

  std::vector<u8_t> png_data;
  png_set_write_fn(writer_.png, &png_data, PNGPlugin::writeData, PNGPlugin::flushData);

  int wdt = descriptorToSave.size.getW();
  int hgt = descriptorToSave.size.getH();

  // Используем 8-битный формат для indexed изображения
  // SGDK сам преобразует 24-битные цвета в 9-битные
  int bit_depth = 8;

  png_set_IHDR(writer_.png, writer_.info, wdt, hgt, bit_depth, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  // Заполняем палитру 24-битными цветами
  PaletteUtil::set(writer_, descriptorToSave);
  PaletteUtil::setTransparency(writer_, descriptorToSave);

  // Записываем информацию.
  png_write_info(writer_.png, writer_.info);

  // Подготавливаем указатели на строки
  const auto &indexedPixels = descriptorToSave.indexed.data;
  std::vector<png_bytep> row_pointers(hgt);

  // Ограничиваем индексы диапазоном палитры
  std::vector<u8_t> image_data;
  image_data.reserve(wdt * hgt);

  for (int i = 0; i < wdt * hgt; ++i) {
    // Ограничиваем индекс размером палитры
    u8_t index = indexedPixels[i];
    if (index >= descriptorToSave.palette.colors.size()) {
      index = 0;  // Fallback to first color if out of range
    }
    image_data.push_back(index);
  }

  // Устанавливаем указатели на строки
  for (int i = 0; i < hgt; ++i) {
    row_pointers[i] = &image_data[i * wdt];
  }

  // Записываем изображение
  png_write_image(writer_.png, row_pointers.data());
  png_write_end(writer_.png, nullptr);

  // Очищаем ресурсы.
  png_destroy_write_struct(&writer_.png, &writer_.info);

  emscripten::val result = emscripten::val::global("Uint8Array")
                               .new_(emscripten::val(emscripten::typed_memory_view(png_data.size(), png_data.data())));

  printf("SGDK-compatible PNG saved successfully: %zu bytes, bit depth: %d, palette: %zu colors (24-bit JASC-PAL)\n",
      png_data.size(), bit_depth, descriptorToSave.palette.colors.size());
  return result;
}

#endif

}  // namespace sway::loader::png
