#pragma once

#include <dubu_vk/dubu_vk.h>
#include <dubu_window/dubu_window.h>

class Application {
public:
	void Run();

private:
	void InitVulkan();
	void MainLoop();

	std::unique_ptr<dubu::window::IWindow> mWindow;

	std::unique_ptr<dubu::vk::Framework> mFramework;
};