#ifndef SWAY_LOADER_PNG_PNGPLUGIN_HPP
#define SWAY_LOADER_PNG_PNGPLUGIN_HPP

#include <sway/core.hpp>
#include <sway/loader.hpp>

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

class PNGPlugin : public ImageLoaderPlugin {
public:
  PNGPlugin();

  virtual ~PNGPlugin() = default;

  [[nodiscard]] virtual std::string getText() const { return "Hello"; }
};

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)

#endif  // SWAY_LOADER_PNG_PNGPLUGIN_HPP
