#pragma once

#include <dubu_gfx/dubu_gfx.h>
#include <dubu_window/dubu_window.h>

class Application : public dubu::event::EventSubscriber {
public:
	void Run();

private:
	void MainLoop();

	void Update();

	void InitFramework();
	void InitImGui();
	void RecreateSwapchain();

	void RecordCommands(uint32_t imageIndex);
	void DrawFrame();

	void CreateInstance();
	void CreateSurface();
	void CreateDevice();
	void CreateSwapchain();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	void CreateFramebuffer();
	void CreateCommandPool();
	void CreateCommandBuffer();
	void CreateSyncObjects();

	std::unique_ptr<dubu::window::GLFWWindow> mWindow;

	std::unique_ptr<dubu::gfx::Instance>         mInstance                 = {};
	std::unique_ptr<dubu::gfx::Surface>          mSurface                  = {};
	std::unique_ptr<dubu::gfx::Device>           mDevice                   = {};
	std::unique_ptr<dubu::gfx::Swapchain>        mSwapchain                = {};
	std::unique_ptr<dubu::gfx::RenderPass>       mRenderPass               = {};
	std::unique_ptr<dubu::gfx::ViewportState>    mViewportState            = {};
	std::unique_ptr<dubu::gfx::GraphicsPipeline> mGraphicsPipeline         = {};
	std::unique_ptr<dubu::gfx::Framebuffer>      mFramebuffer              = {};
	std::unique_ptr<dubu::gfx::CommandPool>      mCommandPool              = {};
	std::unique_ptr<dubu::gfx::CommandBuffer>    mCommandBuffer            = {};
	std::vector<vk::UniqueSemaphore>             mRenderFinishedSemaphores = {};
	std::vector<vk::UniqueSemaphore>             mImageAvailableSemaphores = {};
	std::vector<vk::UniqueFence>                 mInFlightFences           = {};
	std::vector<vk::Fence>                       mImagesInFlight           = {};
	std::size_t                                  mCurrentFrame             = {};
	bool                                         mIsMinimized = false;

	std::unique_ptr<dubu::gfx::DescriptorPool> mImGuiDescriptorPool = {};
	bool                                       mShowDemoWindow      = true;
};