#include "Surface.h"

namespace dubu::gfx {

Surface::Surface(vk::UniqueSurfaceKHR surface)
    : mSurface(std::move(surface)) {}

}  // namespace dubu::gfx
