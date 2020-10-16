#include <iostream>

#define GLFW_INCLUDE_VULKAN

#include <dubu_vk/dubu_vk.h>
#include <dubu_window/dubu_window.h>

int main() {
	dubu::window::GLFWWindow window(600, 600, "dubu-vk");

	auto extensions = vk::enumerateInstanceExtensionProperties();

	std::cout << extensions.size() << " extensions supported." << std::endl;

	while (!window.ShouldClose()) {
		window.PollEvents();
	}
}
