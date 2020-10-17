#pragma once

#include <dubu_gfx/dubu_gfx.h>
#include <dubu_window/dubu_window.h>

class Application {
public:
	void Run();

private:
	void InitVulkan();
	void MainLoop();

	std::unique_ptr<dubu::window::IWindow> mWindow;

	std::unique_ptr<dubu::gfx::Framework> mFramework;
};