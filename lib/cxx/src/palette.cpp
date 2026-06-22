#include <sway/loader/png/defines.hpp>
#include <sway/loader/png/palette.hpp>

#include <fstream>
#include <set>

using namespace sway::core;

namespace sway::loader::png {

/**
 * @brief Конвертирует HEX в RGB.
 *
 * @param[in] hex HEX строка.
 * @return math::Color<u8_t>
 */
auto convHexToRgb(std::string hex) -> math::Color<u8_t> {
  math::Color<u8_t> result;

  if (hex.length() == 7 && hex[0] == '#') {
    auto r = std::stoi(hex.substr(1, 2), nullptr, 16);
    auto g = std::stoi(hex.substr(3, 2), nullptr, 16);
    auto b = std::stoi(hex.substr(5, 2), nullptr, 16);
    result.set(r, g, b, PNG_COLOR_CHANNEL_MAX);
  }

  return result;
}

auto PaletteUtil::hasMagenta(const math::Color<u8_t> &color) -> bool {
  return (color.getR() == PNG_COLOR_CHANNEL_MAX && color.getG() == PNG_COLOR_CHANNEL_MIN &&
          color.getB() == PNG_COLOR_CHANNEL_MAX);
}

void PaletteUtil::get(PNGPluginPrivate reader, Palette &palette) {
  png_colorp embeddedColorPalette = nullptr;
  i32_t nbColorEntries = 0;

  png_bytep embeddedTransPalette = nullptr;
  i32_t nbTransEntries = 0;

  if (!png_get_valid(reader.png, reader.info, PNG_INFO_PLTE)) {
    return;
  }

  if (!png_get_PLTE(reader.png, reader.info, &embeddedColorPalette, &nbColorEntries)) {
    return;
  }

  if (!png_get_tRNS(reader.png, reader.info, &embeddedTransPalette, &nbTransEntries, nullptr)) {
    // no transparency
  }

  palette.colors.resize(nbColorEntries);

  for (i32_t i = 0; i < nbColorEntries; ++i) {
    auto const &color = embeddedColorPalette[i];
    palette.colors[i].set(color.red, color.green, color.blue,
        embeddedTransPalette && i < nbTransEntries ? embeddedTransPalette[i] : PNG_COLOR_CHANNEL_MAX);
  }
}

auto PaletteUtil::findTransparentIndex(const std::vector<math::Color<u8_t>> &colors) -> i32_t {
  i32_t result = -1;
  for (size_t i = 0; i < colors.size(); ++i) {
    const auto &color = colors[i];
    if (PaletteUtil::hasMagenta(color)) {
      result = static_cast<i32_t>(i);
      printf("Transparent color found at index: %d\n", result);
      break;
    }
  }

  return result;
}

void PaletteUtil::setTransparency(PNGPluginPrivate writer, ImageDescriptor &desc) {
  const auto transparentIndex = PaletteUtil::findTransparentIndex(desc.palette.colors);
  // Если нашли прозрачный цвет, устанавливаем прозрачность только для него.
  if (transparentIndex != -1) {
    png_bytep trans = (png_bytep)png_malloc(writer.png, desc.palette.colors.size() * sizeof(png_byte));

    // Инициализируем все цвета как непрозрачные.
    for (size_t i = 0; i < desc.palette.colors.size(); ++i) {
      trans[i] = PNG_COLOR_CHANNEL_MAX;  // 255 = полностью непрозрачный.
    }

    // Устанавливаем только прозрачный цвет как прозрачный.
    trans[transparentIndex] = PNG_COLOR_CHANNEL_MIN;  // 0 = полностью прозрачный.

    png_set_tRNS(writer.png, writer.info, trans, desc.palette.colors.size(), nullptr);
    png_free(writer.png, trans);
  }
}

void PaletteUtil::set(PNGPluginPrivate writer, const ImageDescriptor &desc) {
  // Создаем палитру для PNG (24-битные цвета в формате JASC-PAL).
  auto *palette = (png_colorp)png_malloc(writer.png, desc.palette.colors.size() * sizeof(png_color));
  for (size_t i = 0; i < desc.palette.colors.size(); ++i) {
    const auto &color = desc.palette.colors[i];
    palette[i].red = color.getR();
    palette[i].green = color.getG();
    palette[i].blue = color.getB();
  }

  png_set_PLTE(writer.png, writer.info, palette, desc.palette.colors.size());
  png_free(writer.png, palette);
}

#ifdef EMSCRIPTEN_PLATFORM

void PaletteUtil::setCustom(ImageDescriptor &desc, const emscripten::val &colors) {
  if (!colors.isArray()) {
    return;
  }

  auto length = colors["length"].as<i32_t>();
  if (length == 0) {
    return;
  }

  desc.palette.colors.clear();

  for (auto i = 0; i < length; ++i) {
    auto hexColor = colors[i].as<std::string>();
    auto rgbColor = convHexToRgb(hexColor);

    desc.palette.colors.push_back(rgbColor);
  }

  desc.paletteCount = 1;
  desc.colorsPerPalette = desc.palette.colors.size();
}

#endif

}  // namespace sway::loader::png
