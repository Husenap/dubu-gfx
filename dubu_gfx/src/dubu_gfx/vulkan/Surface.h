#pragma once

namespace dubu::gfx {

class Surface {
public:
	Surface(vk::UniqueSurfaceKHR surface);

	[[nodiscard]] const vk::SurfaceKHR& GetSurface() const { return *mSurface; }

private:
	vk::UniqueSurfaceKHR mSurface;
};

}  // namespace dubu::gfx