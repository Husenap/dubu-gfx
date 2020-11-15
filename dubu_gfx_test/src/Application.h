#pragma once

#include <dubu_gfx/dubu_gfx.h>
#include <dubu_window/dubu_window.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

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
	void UpdateUniformBuffer(uint32_t imageIndex);
	void DrawFrame();

	void CreateInstance();
	void CreateSurface();
	void CreateDevice();
	void CreateSwapchain();
	void CreateRenderPass();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateFramebuffer();
	void CreateCommandPool();
	void CreateDepthResources();
	void CreateTextureImage();
	void CreateModel();
	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateCommandBuffer();
	void CreateSyncObjects();

	std::unique_ptr<dubu::window::GLFWWindow> mWindow;

	std::unique_ptr<dubu::gfx::Instance>            mInstance            = {};
	std::unique_ptr<dubu::gfx::Surface>             mSurface             = {};
	std::unique_ptr<dubu::gfx::Device>              mDevice              = {};
	std::unique_ptr<dubu::gfx::Swapchain>           mSwapchain           = {};
	std::unique_ptr<dubu::gfx::RenderPass>          mRenderPass          = {};
	std::unique_ptr<dubu::gfx::ViewportState>       mViewportState       = {};
	std::unique_ptr<dubu::gfx::DescriptorSetLayout> mDescriptorSetLayout = {};
	vk::UniquePipelineLayout                        mPipelineLayout      = {};
	std::unique_ptr<dubu::gfx::GraphicsPipeline>    mGraphicsPipeline    = {};
	std::unique_ptr<dubu::gfx::Framebuffer>         mFramebuffer         = {};
	std::unique_ptr<dubu::gfx::CommandPool>         mCommandPool         = {};
	std::unique_ptr<dubu::gfx::Image>               mDepthImage          = {};
	std::unique_ptr<Model>                          mModel               = {};
	std::unique_ptr<dubu::gfx::Image>               mTextureImage        = {};
	std::unique_ptr<dubu::gfx::Sampler>             mTextureSampler      = {};
	std::vector<std::unique_ptr<dubu::gfx::Buffer>> mUniformBuffers      = {};
	std::unique_ptr<dubu::gfx::DescriptorPool>      mDescriptorPool      = {};
	std::unique_ptr<dubu::gfx::DescriptorSet>       mDescriptorSet       = {};
	std::unique_ptr<dubu::gfx::CommandBuffer>       mCommandBuffer       = {};
	std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores           = {};
	std::vector<vk::UniqueSemaphore> mImageAvailableSemaphores           = {};
	std::vector<vk::UniqueFence>     mInFlightFences                     = {};
	std::vector<vk::Fence>           mImagesInFlight                     = {};
	std::size_t                      mCurrentFrame                       = {};
	bool                             mIsMinimized = false;

	std::unique_ptr<dubu::gfx::DescriptorPool> mImGuiDescriptorPool = {};
	bool                                       mShowDemoWindow      = false;
};