#ifndef SWAY_LOADER_PNG_PNGPLUGIN_HPP
#define SWAY_LOADER_PNG_PNGPLUGIN_HPP

#include <sway/core.hpp>

#define LOADER_PLUGIN_EXPORT  // empty

#define DECLARE_LOADER_PLUGIN(plugclass, plugname, version)                                                            \
  extern "C" {                                                                                                         \
  LOADER_PLUGIN_EXPORT sway::loader::png::LoaderPlugin *getPlugin() {                                                  \
    static plugclass singleton;                                                                                        \
    return &singleton;                                                                                                 \
  }                                                                                                                    \
  LOADER_PLUGIN_EXPORT sway::loader::png::LoaderPluginInfo exports = {{plugname, "", "", "", "", version}, getPlugin}; \
  }

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

class LoaderPlugin {
public:
};

typedef LoaderPlugin *(*LoaderPluginInitializeFunc_t)();

struct LoaderPluginInfo : public core::PluginInfo {
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
