#ifndef SWAY_LOADER_PNG_PNGPLUGIN_HPP
#define SWAY_LOADER_PNG_PNGPLUGIN_HPP

#include <sway/core.hpp>

#define LOADER_PLUGIN_EXPORT  // empty

#define DECLARE_LOADER_PLUGIN(plugclass, plugname, version)                                                   \
  extern "C" {                                                                                                \
  LOADER_PLUGIN_EXPORT sway::loader::png::LoaderPlugin *getInstance() {                                       \
    static plugclass singleton;                                                                               \
    return &singleton;                                                                                        \
  }                                                                                                           \
  LOADER_PLUGIN_EXPORT sway::core::PluginInfo pluginGetInfo() { return {plugname, "", "", "", "", version}; } \
  LOADER_PLUGIN_EXPORT sway::loader::png::LoaderPluginInfo getExports() { return {getInstance}; }             \
  }

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

class LoaderPlugin {};

typedef LoaderPlugin *(*LoaderPluginInitializeFunc_t)();

struct LoaderPluginInfo {
  LoaderPluginInitializeFunc_t plug;
};

class PNGPlugin : public LoaderPlugin {
public:
  PNGPlugin();

  virtual ~PNGPlugin() = default;
};

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)

#endif  // SWAY_LOADER_PNG_PNGPLUGIN_HPP
