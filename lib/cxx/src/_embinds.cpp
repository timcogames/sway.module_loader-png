#include <sway/loader.hpp>
#include <sway/loader/png/pngplugin.hpp>

using namespace sway::loader;
using namespace sway::loader::png;

#if (defined EMSCRIPTEN_PLATFORM && defined EMSCRIPTEN_USE_BINDINGS)

#  include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(sway_math_size2i) {
  emscripten::class_<sway::math::size2i_t>("Size2i")
      .constructor<>()
      .constructor<int, int>()
      .function("getW", &sway::math::size2i_t::getW)
      .function("getH", &sway::math::size2i_t::getH)
      .function("setW", &sway::math::size2i_t::setW)
      .function("setH", &sway::math::size2i_t::setH);
}

EMSCRIPTEN_BINDINGS(sway_loader_palette) {
  emscripten::class_<sway::math::Color<sway::u8_t>>("Color4I")
      .constructor<>()
      .constructor<sway::u8_t, sway::u8_t, sway::u8_t, sway::u8_t>()
      .function("getR", &sway::math::Color<sway::u8_t>::getR)
      .function("getG", &sway::math::Color<sway::u8_t>::getG)
      .function("getB", &sway::math::Color<sway::u8_t>::getB)
      .function("getA", &sway::math::Color<sway::u8_t>::getA)
      .function("set", &sway::math::Color<sway::u8_t>::set);

  emscripten::register_vector<sway::math::Color<sway::u8_t>>("ColorRGBAVector");

  emscripten::class_<sway::loader::Palette>("Palette").constructor<>().property(
      "colors", &sway::loader::Palette::colors);
}

EMSCRIPTEN_BINDINGS(sway_loader_imagebuffer) {
  emscripten::value_object<ImageBuffer>("ImageBuffer")
      .field("data", &ImageBuffer::data)
      .field("len", &ImageBuffer::len);
}

EMSCRIPTEN_BINDINGS(sway_loader_imagecoloranalysis) {
  emscripten::value_object<ImageColorAnalysis>("ImageColorAnalysis")
      .field("uniqueColors", &ImageColorAnalysis::uniqueColors)
      .field("totalPixels", &ImageColorAnalysis::totalPixels);
}

EMSCRIPTEN_BINDINGS(sway_loader_imagedescriptor) {
  emscripten::value_object<ImageDescriptor>("ImageDescriptor")
      .field("buf", &ImageDescriptor::buf)
      .field("size", &ImageDescriptor::size)
      .field("pitch", &ImageDescriptor::pitch)
      .field("bpp", &ImageDescriptor::bpp)
      .field("bitsPerChannel", &ImageDescriptor::bitsPerChannel)
      .field("channels", &ImageDescriptor::channels)
      .field("colorsPerPalette", &ImageDescriptor::colorsPerPalette)
      .field("palette", &ImageDescriptor::palette)
      .field("paletteCount", &ImageDescriptor::paletteCount)
      .field("colorType", &ImageDescriptor::colorType)
      .field("bitDepth", &ImageDescriptor::bitDepth)
      .field("type", &ImageDescriptor::type)
      .field("format", &ImageDescriptor::format)
      .field("colorAnalysis", &ImageDescriptor::colorAnalysis);
}

EMSCRIPTEN_BINDINGS(sway_loader_png) { PNGPlugin::bindEmscriptenClass(); }

#endif
