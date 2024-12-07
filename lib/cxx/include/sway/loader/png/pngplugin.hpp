#ifndef SWAY_LOADER_PNG_PNGPLUGIN_HPP
#define SWAY_LOADER_PNG_PNGPLUGIN_HPP

#include <sway/core.hpp>
#include <sway/loader.hpp>
#include <sway/math.hpp>

#include <png.h>

NS_BEGIN_SWAY()
NS_BEGIN(loader)
NS_BEGIN(png)

class PNGPlugin final : public ImageLoaderPlugin {
public:
#pragma region "Static methods"

  static void readData(png_structp png, png_bytep data, png_size_t length);

  static void readAsyncData(png_structp png, png_bytep data, png_size_t length);

  static void error(png_structp png, png_const_charp message) {}

  static void warning(png_structp png, png_const_charp message) {}

#pragma endregion

#pragma region "Ctors/Dtor"

  PNGPlugin() = default;

  DTOR_VIRTUAL_DEFAULT(PNGPlugin);

#pragma endregion

#pragma region "Overridden ImageLoaderPlugin methods"

  MTHD_OVERRIDE(auto loadFromStream(std::ifstream &source) -> ImageDescriptor);

  MTHD_OVERRIDE(auto loadFrom(void *buffer, int size) -> ImageDescriptor);

#pragma endregion

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

NS_END()  // namespace png
NS_END()  // namespace loader
NS_END()  // namespace sway

#endif  // SWAY_LOADER_PNG_PNGPLUGIN_HPP
