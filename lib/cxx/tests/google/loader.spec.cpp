#include <sway/core.hpp>
#include <sway/loader/png/pngplugin.hpp>

#include <gtest/gtest.h>

using namespace sway;

TEST(Plug, info) { auto *plug = new loader::png::PNGPlugin(); }
