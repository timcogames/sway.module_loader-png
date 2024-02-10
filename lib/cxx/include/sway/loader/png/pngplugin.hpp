#ifndef SWAY_LOADER_PNG_PNGPLUGIN_HPP
#define SWAY_LOADER_PNG_PNGPLUGIN_HPP

#include <sway/core.hpp>
#include <sway/loader.hpp>
#include <sway/math.hpp>

#include <png.h>

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

class PNGPlugin : public ImageLoaderPlugin {
public:
  PNGPlugin() = default;

  virtual ~PNGPlugin() = default;

  // clang-format off
  MTHD_OVERRIDE(auto loadFromStream(std::ifstream &source) -> ImageDescriptor);  // clang-format on

  // clang-format off
  MTHD_OVERRIDE(auto loadFrom(void *buffer, int size) -> ImageDescriptor);  // clang-format on

  static void readData(png_structp png, png_bytep data, png_size_t length);
  static void readAsyncData(png_structp png, png_bytep data, png_size_t length);

  static void error(png_structp png, png_const_charp message) {}

  static void warning(png_structp png, png_const_charp message) {}

private:
  auto readSignature_nostream_(void *buffer) -> bool;

  void readSignature_(std::ifstream &source);

  auto createInfoStruct_nostream_() -> png_infop;

  auto createInfoStruct_(std::ifstream &source) -> png_infop;

  void create_nostream_();

  void create_(std::ifstream &source);

  void getImageSizeInfo_(math::size2i_t &size);

  png_structp png_;
  png_infop info_;
  png_infop endInfo_;
};

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)

#endif  // SWAY_LOADER_PNG_PNGPLUGIN_HPP
