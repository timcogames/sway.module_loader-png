#ifndef SWAY_LOADER_PNG_PNGPLUGINPRIVATE_HPP
#define SWAY_LOADER_PNG_PNGPLUGINPRIVATE_HPP

#include <png.h>

namespace sway::loader::png {

struct PNGPluginPrivate {
  png_structp png;
  png_infop info;
};

}  // namespace sway::loader::png

#endif  // SWAY_LOADER_PNG_PNGPLUGINPRIVATE_HPP
