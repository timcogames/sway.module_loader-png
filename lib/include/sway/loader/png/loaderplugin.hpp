#ifndef SWAY_LOADER_LOADERPLUGIN_HPP
#define SWAY_LOADER_LOADERPLUGIN_HPP

// #include <sway/core.hpp>

// #include <type_traits>

// #define DEFINE_LOADER_PLUGIN(plugclass) using type_t = plugclass;

// #define DECLARE_LOADER_PLUGIN(plugclass, plugname, version)                                                   \
//   using PluginType_t = plugclass::type_t;                                                              \
//   EXTERN_C_BEGIN                                                                                       \
//   DLLAPI_EXPORT sway::loader::LoaderPlugin *pluginGetInst() {                                          \
//     static plugclass singleton;                                                                        \
//     return &singleton;                                                                                 \
//   }                                                                                                    \
//   DLLAPI_EXPORT sway::core::PluginInfo pluginGetInfo() { return {plugname, "", "", "", "", version}; } \
//   DLLAPI_EXPORT sway::loader::LoaderPluginDescriptor<PluginType_t> pluginGetLoader() {                 \
//     return {{pluginGetInfo()}, (LoaderPluginInistance_t<PluginType_t>)pluginGetInst};                  \
//   }                                                                                                    \
//   EXTERN_C_END

// NAMESPACE_BEGIN(sway)
// NAMESPACE_BEGIN(loader)

// class LoaderPlugin {};

// class ImageLoaderPlugin : public LoaderPlugin {
// public:
//   DEFINE_LOADER_PLUGIN(ImageLoaderPlugin)

//   [[nodiscard]] virtual std::string getText() const = 0;
// };

// class AudioLoaderPlugin : public LoaderPlugin {
// public:
//   DEFINE_LOADER_PLUGIN(AudioLoaderPlugin)

//   [[nodiscard]] virtual std::string getBody() const = 0;
// };

// template <class TConcreteLoader>
// using LoaderPluginInistance_t = TConcreteLoader *(*)();

// template <class TConcreteLoader>
// struct LoaderPluginDescriptor : public sway::core::PluginInfo {
//   LoaderPluginInistance_t<TConcreteLoader> plug;
// };

NAMESPACE_END(loader)
NAMESPACE_END(sway)

#endif  // SWAY_LOADER_LOADERPLUGIN_HPP
