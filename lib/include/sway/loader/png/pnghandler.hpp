#ifndef SWAY_LOADER_PNG_PNGHANDLER_HPP
#define SWAY_LOADER_PNG_PNGHANDLER_HPP

#include <sway/core.hpp>

#include <fstream>  //std::ifstream
#include <png.h>

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

struct ImageDescriptor {
  u8_t *pixels;
  u32_t width;
  u32_t height;
  s32_t pitch;
  s32_t bpp;
  // DATA_TYPE type;
  // PIXEL_FORMAT format;
  // PIXEL_FORMAT internalFormat;
};

class PNGHandler {
public:
  PNGHandler();

  virtual ~PNGHandler() = default;

  auto loadFromStream(std::ifstream &source) -> ImageDescriptor;

  static void readData(png_structp png, png_bytep data, png_size_t length) {}

  static void error(png_structp png, png_const_charp message) {}

  static void warning(png_structp png, png_const_charp message) {}
};

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)

#endif  // SWAY_LOADER_PNG_PNGHANDLER_HPP
