#ifndef SWAY_LOADER_PNG_PNGPLUGIN_HPP
#define SWAY_LOADER_PNG_PNGPLUGIN_HPP

#include <sway/core.hpp>
#include <sway/emscriptenmacros.hpp>
#include <sway/loader.hpp>
#include <sway/loader/png/pngpluginprivate.hpp>
#include <sway/math.hpp>

#include <png.h>

namespace sway::loader::png {

class PNGPlugin final : public ImageLoaderPlugin {
  DECLARE_EMSCRIPTEN_BINDING()

public:
#pragma region "Static methods"

  static void readData(png_structp png, png_bytep data, png_size_t length);

  static void readAsyncData(png_structp png, png_bytep data, png_size_t length);

  static void writeData(png_structp png, png_bytep data, png_size_t length);

  static void flushData(png_structp png);

  static void error(png_structp png, png_const_charp message) {}

  static void warning(png_structp png, png_const_charp message) {}

#pragma endregion

#pragma region "Ctors/Dtor"

  PNGPlugin() = default;

  virtual ~PNGPlugin() = default;

#pragma endregion

#pragma region "Overridden ImageLoaderPlugin methods"

  MTHD_OVERRIDE(auto loadFromStream(std::ifstream &source) -> ImageDescriptor);

  MTHD_OVERRIDE(auto loadFrom(void *buffer, int size) -> ImageDescriptor);

#ifdef EMSCRIPTEN_PLATFORM
  auto loadFromArrayBuffer(emscripten::val buf) -> ImageDescriptor;

  void setCustomPalette(ImageDescriptor &desc, const emscripten::val &colors);

  auto getImageDescriptor() -> ImageDescriptor;

  auto saveToIndexedArrayBuffer(ImageDescriptor &desc) -> emscripten::val;
#endif

#pragma endregion

private:
  auto readSignature_nostream_(void *buffer) -> bool;

  void readSignature_(std::ifstream &source);

  auto createInfoStruct_nostream_() -> png_infop;

  auto createInfoStruct_(std::ifstream &source) -> png_infop;

  void create_nostream_();

  void create_(std::ifstream &source);

  void getImageSizeInfo_(math::size2i_t &size);

  void transformation_(png_byte colorType, png_byte bitDepth, png_byte *channels, png_byte *bitsPerChannel);

  void analyzeColors_(ImageDescriptor &descriptor, u8_t *imgData, const math::size2i_t size, int channels);

  PNGPluginPrivate reader_;
  PNGPluginPrivate writer_;

  ImageDescriptor descriptor_;

  png_infop endInfo_;
};

}  // namespace sway::loader::png

#endif  // SWAY_LOADER_PNG_PNGPLUGIN_HPP
