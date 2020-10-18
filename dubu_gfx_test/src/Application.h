#pragma once

#include <dubu_gfx/dubu_gfx.h>
#include <dubu_window/dubu_window.h>

class Application {
public:
	void Run();

private:
	void InitFramework();
	void MainLoop();

	std::unique_ptr<dubu::window::GLFWWindow> mWindow;

	std::unique_ptr<dubu::gfx::Instance>         mInstance;
	std::unique_ptr<dubu::gfx::Surface>          mSurface;
	std::unique_ptr<dubu::gfx::Device>           mDevice;
	std::unique_ptr<dubu::gfx::Swapchain>        mSwapchain;
	std::unique_ptr<dubu::gfx::GraphicsPipeline> mGraphicsPipeline;
};