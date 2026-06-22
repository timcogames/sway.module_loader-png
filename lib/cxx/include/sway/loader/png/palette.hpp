#ifndef SWAY_LOADER_PNG_PALETTE_HPP
#define SWAY_LOADER_PNG_PALETTE_HPP

#include <sway/core.hpp>
#include <sway/emscriptenmacros.hpp>
#include <sway/loader.hpp>
#include <sway/loader/png/pngpluginprivate.hpp>
#include <sway/math.hpp>

#include <png.h>

namespace sway::loader::png {

class PaletteUtil final {
public:
#pragma region "Static methods"

  static void get(PNGPluginPrivate reader, Palette &palette);

  static void set(PNGPluginPrivate writer, const ImageDescriptor &desc);

  static auto hasMagenta(const math::Color<u8_t> &color) -> bool;

  // Находим индекс прозрачного цвета (#ff00ff).
  static auto findTransparentIndex(const std::vector<math::Color<u8_t>> &colors) -> i32_t;

  static void setTransparency(PNGPluginPrivate writer, ImageDescriptor &desc);

#ifdef EMSCRIPTEN_PLATFORM

  static void setCustom(ImageDescriptor &desc, const emscripten::val &colors);

#endif

#pragma endregion
};

}  // namespace sway::loader::png

#endif  // SWAY_LOADER_PNG_PALETTE_HPP
