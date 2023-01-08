#include <sway/loader/png/pngplugin.hpp>

#include <iostream>

NAMESPACE_BEGIN(sway)
NAMESPACE_BEGIN(loader)
NAMESPACE_BEGIN(png)

DECLARE_LOADER_PLUGIN(PNGPlugin, "png", core::Version(1))

PNGPlugin::PNGPlugin() { std::cout << "ctor" << std::endl; }

NAMESPACE_END(png)
NAMESPACE_END(loader)
NAMESPACE_END(sway)